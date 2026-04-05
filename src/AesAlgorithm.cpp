#include "AesAlgorithm.h"
#include <cryptopp/aes.h>
#include <cryptopp/gcm.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>
#include <cryptopp/secblock.h>
#include <stdexcept>

bool AesAlgorithm::deriveKey(const QString& password, const QByteArray& salt, QByteArray& outKey) const {
    try {
        CryptoPP::SecByteBlock key(32);
        const QByteArray passUtf8 = password.toUtf8();
        CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> pbkdf2;
        pbkdf2.DeriveKey(key, key.size(), 0,
                         reinterpret_cast<const CryptoPP::byte*>(passUtf8.constData()),
                         static_cast<size_t>(passUtf8.size()),
                         reinterpret_cast<const CryptoPP::byte*>(salt.constData()),
                         static_cast<size_t>(salt.size()), 200000);
        outKey = QByteArray(reinterpret_cast<const char*>(key.data()), key.size());
        return true;
    } catch (...) { return false; }
}

QByteArray AesAlgorithm::encrypt(const QByteArray& plain, const QByteArray& key, const QByteArray& iv) const {
    CryptoPP::SecByteBlock k(reinterpret_cast<const CryptoPP::byte*>(key.constData()), key.size());
    CryptoPP::SecByteBlock v(reinterpret_cast<const CryptoPP::byte*>(iv.constData()), iv.size());
    CryptoPP::GCM<CryptoPP::AES>::Encryption enc;
    enc.SetKeyWithIV(k, k.size(), v, v.size());

    std::string ct(plain.size(), '\0');
    enc.ProcessData(reinterpret_cast<CryptoPP::byte*>(&ct[0]),
                    reinterpret_cast<const CryptoPP::byte*>(plain.constData()),
                    static_cast<size_t>(plain.size()));

    CryptoPP::byte tag[16];
    enc.TruncatedFinal(tag, 16);
    return QByteArray::fromStdString(ct) + QByteArray(reinterpret_cast<const char*>(tag), 16);
}

QByteArray AesAlgorithm::decrypt(const QByteArray& cipherWithTag, const QByteArray& key, const QByteArray& iv) const {
    if (cipherWithTag.size() < 16) throw std::runtime_error("Ciphertext too short");

    CryptoPP::SecByteBlock k(reinterpret_cast<const CryptoPP::byte*>(key.constData()), key.size());
    CryptoPP::SecByteBlock v(reinterpret_cast<const CryptoPP::byte*>(iv.constData()), iv.size());
    CryptoPP::GCM<CryptoPP::AES>::Decryption dec;
    dec.SetKeyWithIV(k, k.size(), v, v.size());

    std::string ct = cipherWithTag.left(cipherWithTag.size() - 16).toStdString();
    std::string tag = cipherWithTag.right(16).toStdString();
    std::string dt(ct.size(), '\0');

    dec.ProcessData(reinterpret_cast<CryptoPP::byte*>(&dt[0]),
                    reinterpret_cast<const CryptoPP::byte*>(ct.data()),
                    ct.size());
    dec.TruncatedFinal(reinterpret_cast<CryptoPP::byte*>(tag.data()), 16);
    return QByteArray::fromStdString(dt);
}
