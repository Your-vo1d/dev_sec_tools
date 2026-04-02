#include "console_logger.h"
#include <QDateTime>
#include <QTextStream>
#include <iostream>

void ConsoleLogger::log(LogLevel level, const QString &message) const {
    QMutexLocker locker(&m_mutex);

    const char* prefix = "";
    switch(level) {
    case LogLevel::Debug:   prefix = "[DEBUG]"; break;
    case LogLevel::Info:    prefix = "[INFO] "; break;
    case LogLevel::Warning: prefix = "[WARN] "; break;
    case LogLevel::Error:   prefix = "[ERROR]"; break;
    }

    // Ошибки и предупреждения -> stderr, остальное -> stdout
    FILE* stream = (level >= LogLevel::Warning) ? stderr : stdout;
    QTextStream out(stream);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
        << " " << prefix << " " << message << "\n";
}
