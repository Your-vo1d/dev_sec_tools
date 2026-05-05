# Общие настройки
CONFIG += c++17 console
INCLUDEPATH += include
LIBS += -lcryptopp

COMMON_SOURCES = \
    src/ConsoleLogger.cpp \
    src/CryptoPPStrategy.cpp \
    src/RecursivePathStepper.cpp \
    src/CryptoManager.cpp

COMMON_HEADERS = \
    include/ConsoleLogger.h \
    include/CryptoPPStrategy.h \
    include/RecursivePathStepper.h \
    include/CryptoManager.h \
    include/CryptoConfig.h \
    include/ILogger.h \
    include/ICryptoStrategy.h \
    include/IPathStepper.h

# По умолчанию собирается основное приложение
build_tests {
    TARGET = crypto_tests
    QT += core testlib
    CONFIG -= cmdline
    DEFINES += UNIT_TESTS
    SOURCES = $$COMMON_SOURCES src/tests.cpp
    HEADERS = $$COMMON_HEADERS
} else {
    TARGET = crypto_app
    QT += core
    CONFIG += cmdline
    SOURCES = $$COMMON_SOURCES src/main.cpp
    HEADERS = $$COMMON_HEADERS
}

 CONFIG+=build_tests

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
