#ifndef CRYPTO_MANAGER_H
#define CRYPTO_MANAGER_H

#include <QString>

class CryptoManager final
{
public:
    // Получение единственного экземпляра
    static CryptoManager &instance();

    // Шифрование всех файлов в папке и подпапках
    bool encryptFolder(const QString &folderPath, const QString &password);

    // Дешифрование всех файлов в папке и подпапках
    bool decryptFolder(const QString &folderPath, const QString &password);

private:
    // Конструктор
    CryptoManager() = default;
    // Деструктор
    ~CryptoManager() = default;

    // Запрет копирования и присвоения
    CryptoManager(const CryptoManager &) = delete;
    CryptoManager &operator=(const CryptoManager &) = delete;

    // Шифрование одного файла
    bool encryptFile(const QString &filePath, const QString &password);
    // Дешифрование одного файла
    bool decryptFile(const QString &filePath, const QString &password);

    // Проверка, был ли файл зашифрован нашей программой
    bool isEncryptedFile(const QString &filePath) const;

    // Проверка существования и корректности директории
    static bool isValidDirectory(const QString &path);
};

#endif // CRYPTO_MANAGER_H
