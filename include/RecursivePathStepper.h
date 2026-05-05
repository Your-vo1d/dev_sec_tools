#ifndef RECURSIVEPATHSTEPPER_H
#define RECURSIVEPATHSTEPPER_H


#include <QDirIterator>
#include "IPathStepper.h"


class RecursivePathStepper : public IPathStepper {
public:
    std::vector<QString> collectFiles(const QString& root) const override;
};


#endif // RECURSIVEPATHSTEPPER_H
