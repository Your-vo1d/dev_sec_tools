#pragma once
#include <QString>
#include <QStringList>

class IDirectoryWalker {
public:
    virtual ~IDirectoryWalker() = default;
    virtual QStringList getFiles(const QString& rootDir) const = 0;
};