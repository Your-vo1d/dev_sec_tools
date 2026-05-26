#ifndef CRYPTO_MANAGER_H
#define CRYPTO_MANAGER_H

#include <QString>
#include "ILogger.h"
#include "ICryptoAlgorithm.h"


class CryptoManager final
{
public:
    // Получение единственного экземпляра
    static CryptoManager &instance();
    void init(ILogger* log, ICryptoAlgorithm* algo);
    // Шифрование всех файлов в папке и подпапках
    bool encryptFile(const QString &folderPath, const QString &password);

    // Дешифрование всех файлов в папке и подпапках
    bool decryptFile(const QString &folderPath, const QString &password);

#ifdef UNIT_TESTS
    // Только для тестов: сброс состояния синглтона между тест-кейсами
    static void resetForTesting() {
        instance().logger = nullptr;
        instance().algorithm = nullptr;
    }
    friend class CryptoTestSuite;
#endif

private:
    // Конструктор
    CryptoManager() = default;
    // Деструктор
    ~CryptoManager() = default;

    // Запрет копирования и присвоения
    CryptoManager(const CryptoManager &) = delete;
    CryptoManager &operator=(const CryptoManager &) = delete;

    ILogger* logger = nullptr;
    ICryptoAlgorithm* algorithm = nullptr;
};



#endif // CRYPTO_MANAGER_H
