#include "CryptoPPStrategy.h"
#include <secblock.h>


QString CryptoPPStrategy::name() const { return "AES-256-CBC"; }

QByteArray CryptoPPStrategy::computeHash(const QString& password) const {
    QByteArray pwdUtf8 = password.toUtf8();
    QByteArray hash(CryptoConfig::HASH_SIZE, 0);
    CryptoPP::SHA256().CalculateDigest(
        reinterpret_cast<CryptoPP::byte*>(hash.data()),
        reinterpret_cast<const CryptoPP::byte*>(pwdUtf8.constData()),
        static_cast<size_t>(pwdUtf8.size())
        );
    return hash;
}

bool CryptoPPStrategy::isEncryptedFile(const QString& path) const {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QByteArray header = f.read(4);
    f.close();
    return (header.size() == 4 && header == QByteArray(CryptoConfig::MAGIC_HEADER));
}

bool CryptoPPStrategy::encrypt(const QString& path, const QString& password) {
    if (isEncryptedFile(path)) return false;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QByteArray plainQt = f.readAll();
    f.close();

    QByteArray key = computeHash(password);
    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::SecByteBlock iv(CryptoConfig::IV_SIZE);
    rng.GenerateBlock(iv, iv.size());

    std::string cipher;
    try {
        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption enc;
        enc.SetKeyWithIV(
            reinterpret_cast<const CryptoPP::byte*>(key.constData()),
            key.size(),
            iv
            );

        // Идиоматичный pipeline: автоматическое выделение буфера и обработка паддинга
        CryptoPP::StringSource(
            reinterpret_cast<const CryptoPP::byte*>(plainQt.constData()),
            plainQt.size(),
            true,
            new CryptoPP::StreamTransformationFilter(
                enc,
                new CryptoPP::StringSink(cipher)
                )
            );
    } catch (const CryptoPP::Exception&) {
        return false;
    }

    QFile wf(path);
    if (!wf.open(QIODevice::WriteOnly)) return false;
    wf.write(QByteArray(CryptoConfig::MAGIC_HEADER));
    wf.write(computeHash(password));
    wf.write(QByteArray(reinterpret_cast<char*>(iv.data()), CryptoConfig::IV_SIZE));
    wf.write(cipher.data(), static_cast<qint64>(cipher.size()));
    wf.close();
    return true;
}

bool CryptoPPStrategy::decrypt(const QString& path, const QString& password) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QByteArray data = f.readAll();
    f.close();

    const size_t headerSize = 4 + CryptoConfig::HASH_SIZE + CryptoConfig::IV_SIZE;
    if (data.size() < headerSize || data.left(4) != QByteArray(CryptoConfig::MAGIC_HEADER)) {
        return false;
    }

    QByteArray storedHash = data.mid(4, CryptoConfig::HASH_SIZE);
    if (storedHash != computeHash(password)) return false;

    QByteArray iv = data.mid(4 + CryptoConfig::HASH_SIZE, CryptoConfig::IV_SIZE);
    QByteArray cipherQt = data.mid(headerSize);
    QByteArray key = computeHash(password);

    std::string plain;
    try {
        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption dec;
        dec.SetKeyWithIV(
            reinterpret_cast<const CryptoPP::byte*>(key.constData()),
            key.size(),
            reinterpret_cast<const CryptoPP::byte*>(iv.constData())
            );

        CryptoPP::StringSource(
            reinterpret_cast<const CryptoPP::byte*>(cipherQt.constData()),
            cipherQt.size(),
            true,
            new CryptoPP::StreamTransformationFilter(
                dec,
                new CryptoPP::StringSink(plain)
                )
            );
    } catch (const CryptoPP::Exception&) {
        return false;
    }

    QFile wf(path);
    if (!wf.open(QIODevice::WriteOnly)) return false;
    wf.write(plain.data(), static_cast<qint64>(plain.size()));
    wf.close();
    return true;
}
