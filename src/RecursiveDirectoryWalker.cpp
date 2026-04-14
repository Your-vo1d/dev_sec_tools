#include "RecursiveDirectoryWalker.h"
#include <QDirIterator>

QStringList RecursiveDirectoryWalker::getFiles(const QString &rootDir) const {
    QStringList files;
    QDirIterator it(rootDir, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) files << it.next();
    return files;
}
