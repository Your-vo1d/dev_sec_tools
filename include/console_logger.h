#pragma once
#include "logger.h"
#include <QMutex>

class ConsoleLogger : public ILogger {
public:
    void log(LogLevel level, const QString &message) const override;
private:
    mutable QMutex m_mutex;
};