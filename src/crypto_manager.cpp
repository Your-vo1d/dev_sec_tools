#include "crypto_manager.h"

#include <QByteArray>
#include <QDirIterator>
#include <QFile>
#include <QSaveFile>
#include <QDebug>

#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/gcm.h>
#include <cryptopp/osrng.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/secblock.h>
#include <cryptopp/sha.h>

namespace
{
const QByteArray kMagic("QTAES1\0\0", 8);

constexpr int kSaltSize = 16;
constexpr int kIvSize = 12;
constexpr int kTagSize = 16;
constexpr int kKeySize = 32; // AES-256
constexpr int kPbkdf2Iterations = 200000;

bool deriveKeyFromPassword(const QString &password,
                           const CryptoPP::byte *salt,
                           size_t saltLen,
                           CryptoPP::SecByteBlock &outKey)
{
    try {
        outKey = CryptoPP::SecByteBlock(kKeySize);

        const QByteArray passUtf8 = password.toUtf8();

        CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> pbkdf2;
        pbkdf2.DeriveKey(outKey,
                         outKey.size(),
                         0,
                         reinterpret_cast<const CryptoPP::byte *>(passUtf8.constData()),
                         static_cast<size_t>(passUtf8.size()),
                         salt,
                         saltLen,
                         kPbkdf2Iterations);
        return true;
    } catch (...) {
        return false;
    }
}
} // namespace


CryptoManager &CryptoManager::instance()
{
    static CryptoManager inst;
    return inst;
}

bool CryptoManager::encryptFolder(const QString &folderPath, const QString &password)
{
    QDirIterator it(folderPath,
                    QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);

    bool allOk = true;

    while (it.hasNext()) {
        const QString filePath = it.next();

        if (isEncryptedFile(filePath)) {
            continue;
        }

        if (!encryptFile(filePath, password)) {
            allOk = false;
            qWarning() << "Encrypt failed:" << filePath;
        } else {
            qInfo() << "Encrypted:" << filePath;
        }
    }

    return allOk;
}

bool CryptoManager::decryptFolder(const QString &folderPath, const QString &password)
{
    QDirIterator it(folderPath,
                    QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);

    bool allOk = true;

    while (it.hasNext()) {
        const QString filePath = it.next();

        if (!isEncryptedFile(filePath)) {
            continue;
        }

        if (!decryptFile(filePath, password)) {
            allOk = false;
            qWarning() << "Decrypt failed:" << filePath;
        } else {
            qInfo() << "Decrypted:" << filePath;
        }
    }

    return allOk;
}

bool CryptoManager::encryptFile(const QString &filePath, const QString &password)
{
    QFile in(filePath);
    if (!in.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot read file:" << filePath;
        return false;
    }

    const QByteArray plain = in.readAll();
    in.close();


    if (plain.isEmpty()) {
        qWarning() << "Empty file, skipping encryption";
        return true;
    }
    if (isEncryptedFile(filePath)) {
        qInfo() << "File already encrypted:" << filePath;
        return true;  // Считаем успешным, просто пропускаем
    }
    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::SecByteBlock salt(kSaltSize);
    CryptoPP::SecByteBlock iv(kIvSize);
    prng.GenerateBlock(salt, salt.size());
    prng.GenerateBlock(iv, iv.size());

    CryptoPP::SecByteBlock key;
    if (!deriveKeyFromPassword(password, salt, salt.size(), key)) {
        qWarning() << "Key derivation failed";
        return false;
    }

    QByteArray cipherPlusTag;
    try {
        CryptoPP::GCM<CryptoPP::AES>::Encryption enc;
        enc.SetKeyWithIV(key, key.size(), iv, iv.size());

        // 1. Выделяем память под шифротекст
        std::string ciphertext(plain.size(), '\0');

        // 2. Шифруем данные напрямую
        enc.ProcessData(
            reinterpret_cast<CryptoPP::byte*>(&ciphertext[0]),
            reinterpret_cast<const CryptoPP::byte*>(plain.constData()),
            static_cast<size_t>(plain.size()));

        // 3. Получаем тег аутентификации
        CryptoPP::byte tag[kTagSize];
        enc.TruncatedFinal(tag, kTagSize);

        // 4. Склеиваем шифротекст и тег
        cipherPlusTag = QByteArray::fromStdString(ciphertext) +
                        QByteArray(reinterpret_cast<char*>(tag), kTagSize);


    } catch (const CryptoPP::Exception& e) {
        qWarning() << "Crypto++ Exception:" << e.what();
        return false;
    } catch (...) {
        qWarning() << "Unknown exception";
        return false;
    }

    if (cipherPlusTag.size() != plain.size() + kTagSize) {
        qWarning() << "Size mismatch!";
        return false;
    }

    QSaveFile out(filePath);
    if (!out.open(QIODevice::WriteOnly)) {
        return false;
    }

    out.write(kMagic);
    out.write(reinterpret_cast<const char *>(salt.data()), salt.size());
    out.write(reinterpret_cast<const char *>(iv.data()), iv.size());
    out.write(cipherPlusTag);

    return out.commit();
}

bool CryptoManager::decryptFile(const QString &filePath, const QString &password)
{
    QFile in(filePath);
    if (!in.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << filePath;
        return false;
    }

    // 1. Проверка Magic
    QByteArray magic = in.read(kMagic.size());
    if (magic != kMagic) {
        qWarning() << "Invalid Magic Header";
        in.close();
        return false;
    }

    // 2. Чтение Salt
    CryptoPP::SecByteBlock salt(kSaltSize);
    if (in.read(reinterpret_cast<char *>(salt.data()), kSaltSize) != kSaltSize) {
        qWarning() << "Failed to read Salt";
        return false;
    }

    // 3. Чтение IV
    CryptoPP::SecByteBlock iv(kIvSize);
    if (in.read(reinterpret_cast<char *>(iv.data()), kIvSize) != kIvSize) {
        qWarning() << "Failed to read IV";
        return false;
    }

    // 4. Чтение данных (шифротекст + тег)
    QByteArray cipherPlusTag = in.readAll();
    in.close();

    if (cipherPlusTag.size() < kTagSize) {
        qWarning() << "Data too short:" << cipherPlusTag.size();
        return false;
    }

    // 5. Деривация ключа
    CryptoPP::SecByteBlock key;
    if (!deriveKeyFromPassword(password, salt, salt.size(), key)) {
        qWarning() << "Key derivation failed";
        return false;
    }

    // 6. Дешифрование
    try {
        CryptoPP::GCM<CryptoPP::AES>::Decryption dec;
        dec.SetKeyWithIV(key, key.size(), iv, iv.size());

        // Разделяем шифротекст и тег
        std::string ciphertext = cipherPlusTag.left(cipherPlusTag.size() - kTagSize).toStdString();
        std::string tag = cipherPlusTag.right(kTagSize).toStdString();

        std::string decryptedRaw(ciphertext.size(), '\0');

        // Дешифруем данные
        dec.ProcessData(
            reinterpret_cast<CryptoPP::byte*>(&decryptedRaw[0]),
            reinterpret_cast<const CryptoPP::byte*>(ciphertext.data()),
            ciphertext.size());

        dec.TruncatedFinal(
            reinterpret_cast<CryptoPP::byte*>(tag.data()),
            kTagSize);

        QByteArray plain = QByteArray::fromStdString(decryptedRaw);

        // 7. Запись
        QSaveFile outFile(filePath);
        if (!outFile.open(QIODevice::WriteOnly)) {
            qWarning() << "Cannot write file:" << filePath;
            return false;
        }
        outFile.write(plain);
        if (!outFile.commit()) {
            qWarning() << "Failed to commit file:" << filePath;
            return false;
        }

        qInfo() << "Decrypted:" << filePath;
        return true;

    } catch (const CryptoPP::Exception& e) {
        qWarning() << "Crypto++ Exception:" << e.what();
        qWarning() << "Possible causes: Wrong password, file corrupted, or encrypted twice";
        return false;
    } catch (...) {
        qWarning() << "Unknown Exception";
        return false;
    }
}

bool CryptoManager::isEncryptedFile(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    // 1. Проверяем размер файла
    if (file.size() < (kMagic.size() + kSaltSize + kIvSize + kTagSize)) {
        file.close();
        return false;
    }

    // 2. Проверяем Magic header
    QByteArray header = file.read(kMagic.size());
    if (header.size() != kMagic.size() || header != kMagic) {
        file.close();
        return false;
    }

    QByteArray salt = file.read(kSaltSize);
    QByteArray iv = file.read(kIvSize);

    file.close();

    // Если salt или iv не прочитались полностью — файл повреждён
    if (salt.size() != kSaltSize || iv.size() != kIvSize) {
        return false;
    }

    return true;
}
