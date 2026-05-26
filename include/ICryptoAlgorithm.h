#ifndef ICRYPTOALGORITHM_H
#define ICRYPTOALGORITHM_H

#include <QString>


class ICryptoAlgorithm {
public:
    virtual ~ICryptoAlgorithm() = default;
    virtual bool encrypt(const QString& path, const QString& password) = 0;
    virtual bool decrypt(const QString& path, const QString& password) = 0;
    virtual bool isEncryptedFile(const QString& path) const = 0;
    virtual QString name() const = 0;
};


#endif // ICRYPTOALGORITHM_H
