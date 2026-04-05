#pragma once
#include "ICryptoAlgorithm.h"

class AesAlgorithm : public ICryptoAlgorithm {
public:
    bool deriveKey(const QString& password, const QByteArray& salt, QByteArray& outKey) const override;
    QByteArray encrypt(const QByteArray& plain, const QByteArray& key, const QByteArray& iv) const override;
    QByteArray decrypt(const QByteArray& cipherWithTag, const QByteArray& key, const QByteArray& iv) const override;
};
