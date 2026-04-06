#include "IDirectoryWalker.h"
#include <QDirIterator>

class RecursiveDirectoryWalker : public IDirectoryWalker {
public:
    QStringList getFiles(const QString& rootDir) const override {
        QStringList files;
        QDirIterator it(rootDir, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (it.hasNext()) files << it.next();
        return files;
    }
};