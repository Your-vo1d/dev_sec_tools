#ifndef IFILEREFRESHER_H
#define IFILEREFRESHER_H

class IFileRefresher
{
public:
    virtual ~IFileRefresher() = default;
    virtual void refresh() = 0;
};

#endif // IFILEREFRESHER_H
