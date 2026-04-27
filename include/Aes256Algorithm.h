#ifndef AES256ALGORITHM_H
#define AES256ALGORITHM_H

#include "ICryptoAlgorithm.h"
#include "CryptoConfig.h"
#include <QByteArray>
#include <QFile>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>
#include <cryptopp/osrng.h>


class Aes256Algorithm : public ICryptoAlgorithm {
public:
    QString name() const override;
    bool encrypt(const QString& path, const QString& password) override;
    bool decrypt(const QString& path, const QString& password) override;
    bool isEncryptedFile(const QString& path) const override;

private:
    QByteArray computeHash(const QString& password) const;
};

#endif // AES256ALGORITHM_H
