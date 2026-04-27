#include "ConsoleLogger.h"
#include <QTextStream>
#include <cstdio>


void ConsoleLogger::info(const QString& msg) {
    QTextStream out(stdout);
    out << "[INFO] " << msg << Qt::endl;
}

void ConsoleLogger::error(const QString& msg) {
    QTextStream err(stderr);
    err << "[ERROR] " << msg << Qt::endl;
}
