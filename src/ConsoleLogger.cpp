#include "ConsoleLogger.h"
#include <QTextStream>
#include <cstdio>


void ConsoleLogger::info(const QString& msg) {
    QTextStream out(stdout);
    out << "[INFO] " << msg << Qt::endl;
}
