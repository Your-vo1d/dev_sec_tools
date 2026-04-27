#ifndef IPATHSTEPPER_H
#define IPATHSTEPPER_H

#include <QString>
#include <vector>


class IPathStepper {
public:
    virtual ~IPathStepper() = default;
    virtual std::vector<QString> collectFiles(const QString& root) const = 0;
};


#endif // IPATHSTEPPER_H
