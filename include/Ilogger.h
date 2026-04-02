#pragma once
#include <QString>
#include <memory>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};


class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(LogLevel level, const QString &message) const = 0;

    virtual void debug(const QString &message) const { log(LogLevel::Debug, message); }
    virtual void info(const QString &message) const { log(LogLevel::Info, message); }
    virtual void warning(const QString &message) const { log(LogLevel::Warning, message); }
    virtual void error(const QString &message) const { log(LogLevel::Error, message); }
};
