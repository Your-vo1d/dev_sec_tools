QT = core

CONFIG += c++17 cmdline

INCLUDEPATH += include
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/Aes256Algorithm.cpp \
        src/ConsoleLogger.cpp \
        src/RecursivePathStepper.cpp \
        src/CryptoManager.cpp \
        src/main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/Aes256Algorithm.h \
    include/ConsoleLogger.h \
    include/CryptoConfig.h \
    include/ICryptoAlgorithm.h \
    include/ILogger.h \
    include/IPathStepper.h \
    include/CryptoManager.h \
    include/RecursivePathStepper.h

LIBS += -lcryptopp
