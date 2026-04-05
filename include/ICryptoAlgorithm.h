#pragma once
#include <QString>
#include <QByteArray>

class ICryptoAlgorithm {
public:
    virtual ~ICryptoAlgorithm() = default;
    virtual bool deriveKey(const QString& password, const QByteArray& salt, QByteArray& outKey) const = 0;
    virtual QByteArray encrypt(const QByteArray& plain, const QByteArray& key, const QByteArray& iv) const = 0;
    virtual QByteArray decrypt(const QByteArray& cipherWithTag, const QByteArray& key, const QByteArray& iv) const = 0;
};
