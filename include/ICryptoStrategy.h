#ifndef ICRYPTOSTRATEGY_H
#define ICRYPTOSTRATEGY_H

#include <QString>


class ICryptoStrategy {
public:
    virtual ~ICryptoStrategy() = default;
    virtual bool encrypt(const QString& path, const QString& password) = 0;
    virtual bool decrypt(const QString& path, const QString& password) = 0;
    virtual bool isEncryptedFile(const QString& path) const = 0;
    virtual QString name() const = 0;
};


#endif // ICRYPTOSTRATEGY_H
