#pragma once
#include "IDirectoryWalker.h"

class RecursiveDirectoryWalker : public IDirectoryWalker {
public:
    QStringList getFiles(const QString &rootDir) const override;
};
