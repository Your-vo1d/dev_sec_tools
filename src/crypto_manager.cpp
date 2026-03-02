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
        return false;
    }

    const QByteArray plain = in.readAll();
    in.close();

    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::SecByteBlock salt(kSaltSize);
    CryptoPP::SecByteBlock iv(kIvSize);
    prng.GenerateBlock(salt, salt.size());
    prng.GenerateBlock(iv, iv.size());

    CryptoPP::SecByteBlock key;
    if (!deriveKeyFromPassword(password, salt, salt.size(), key)) {
        return false;
    }

    QByteArray cipherPlusTag;
    try {
        CryptoPP::GCM<CryptoPP::AES>::Encryption enc;
        enc.SetKeyWithIV(key, key.size(), iv, iv.size());

        std::string out;
        CryptoPP::AuthenticatedEncryptionFilter aef(enc,
                                                    new CryptoPP::StringSink(out),
                                                    false,
                                                    kTagSize);

        aef.Put(reinterpret_cast<const CryptoPP::byte *>(plain.constData()),
                static_cast<size_t>(plain.size()));
        aef.MessageEnd();

        cipherPlusTag = QByteArray::fromStdString(out);
    } catch (...) {
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

bool CryptoManager::isEncryptedFile(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray header = file.read(9);
    file.close();

    if (header.size() != 9)
        return false;

    if (header.left(8) != kMagic)
        return false;

    return true;
}
