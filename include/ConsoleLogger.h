#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H


#include "ILogger.h"
#include <QString>

class ConsoleLogger : public ILogger {
public:
    ConsoleLogger() = default;

    void info(const QString& msg) override;
};


#endif // CONSOLELOGGER_H
