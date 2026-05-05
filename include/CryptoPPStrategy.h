#ifndef CRYPTOPPSTRATEGY_H
#define CRYPTOPPSTRATEGY_H

#include "ICryptoStrategy.h"
#include "CryptoConfig.h"
#include <QByteArray>
#include <QFile>
#include <aes.h>
#include <modes.h>
#include <filters.h>
#include <sha.h>
#include <osrng.h>


class CryptoPPStrategy : public ICryptoStrategy {
public:
    QString name() const override;
    bool encrypt(const QString& path, const QString& password) override;
    bool decrypt(const QString& path, const QString& password) override;
    bool isEncryptedFile(const QString& path) const override;

private:
    QByteArray computeHash(const QString& password) const;
};

#endif // CRYPTOPPSTRATEGY_H
