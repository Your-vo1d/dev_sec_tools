#include <QCoreApplication>
#include <QFileInfo>
#include <QTextStream>
#include <cstdio>

#include "ConsoleLogger.h"
#include "Aes256Algorithm.h"
#include "RecursivePathStepper.h"
#include "CryptoManager.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    if (argc != 4) {
        QTextStream err(stderr);
        err << "Usage: " << argv[0] << " <path> <encrypt|decrypt> <password>" << Qt::endl;
        return 1;
    }

    QString path = QString::fromLocal8Bit(argv[1]);
    QString mode = QString::fromLocal8Bit(argv[2]).toLower();
    QString password = QString::fromLocal8Bit(argv[3]);

    if (mode != "encrypt" && mode != "decrypt") {
        QTextStream err(stderr);
        err << "Invalid mode. Use 'encrypt' or 'decrypt'." << Qt::endl;
        return 1;
    }

    bool encryptMode = (mode == "encrypt");

    // Dependency Injection
    ConsoleLogger logger;
    Aes256Algorithm algo;
    RecursivePathStepper stepper;

    CryptoManager::instance().init(&logger, &algo);

    QFileInfo fi(path);
    if (!fi.exists()) {
        logger.error(QString("Target path does not exist: %1").arg(path));
        return 1;
    }

    std::vector<QString> filesToProcess;
    if (fi.isFile()) {
        filesToProcess.push_back(path);
    } else if (fi.isDir()) {
        filesToProcess = stepper.collectFiles(path);
        if (filesToProcess.empty()) {
            logger.info("Directory is empty. Nothing to process.");
            return 0;
        }
    }

    logger.info(QString("Processing %1 file(s) with algorithm: %2")
                    .arg(filesToProcess.size())
                    .arg(algo.name()));

    // Обход и обработка каждого файла
    for (const auto& file : filesToProcess) {
        if (encryptMode) {
            CryptoManager::instance().encryptFile(file, password);
        } else {
            CryptoManager::instance().decryptFile(file, password);
        }
    }

    return 0;
}
