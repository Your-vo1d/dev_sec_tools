#include <QFileInfo>
#include "CryptoManager.h"

CryptoManager& CryptoManager::instance() {
    static CryptoManager inst;
    return inst;
}

void CryptoManager::init(ILogger* log, ICryptoAlgorithm* algo) {
    logger = log;
    algorithm = algo;
}

bool CryptoManager::encryptFile(const QString& path, const QString& password) {
    if (!QFileInfo::exists(path)) {
        logger->error(QString("Path does not exist: %1").arg(path));
        return false;
    }
    if (algorithm->isEncryptedFile(path)) {
        logger->error(QString("File is already encrypted. Re-encryption prohibited: %1").arg(path));
        return false;
    }

    bool success = algorithm->encrypt(path, password);
    logger->info(success ? QString("Encryption successful: %1").arg(path)
                         : QString("Encryption failed: %1").arg(path));
    return success;
}

bool CryptoManager::decryptFile(const QString& path, const QString& password) {
    if (!QFileInfo::exists(path)) {
        logger->error(QString("Path does not exist: %1").arg(path));
        return false;
    }
    if (!algorithm->isEncryptedFile(path)) {
        logger->error(QString("File is not encrypted. Cannot decrypt: %1").arg(path));
        return false;
    }

    bool success = algorithm->decrypt(path, password);
    logger->info(success ? QString("Decryption successful: %1").arg(path)
                         : QString("Decryption failed (wrong key or corrupt data): %1").arg(path));
    return success;
}
