#include "RecursivePathStepper.h"
#include <QDir>

std::vector<QString> RecursivePathStepper::collectFiles(const QString& root) const {
    std::vector<QString> files;
    QDir dir(root);
    if (!dir.exists()) return files;

    // Рекурсивный обход, возвращаем только файлы
    QDirIterator it(root, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        files.push_back(it.filePath());
    }
    return files;
}
