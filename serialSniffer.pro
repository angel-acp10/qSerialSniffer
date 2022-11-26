QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    SettingsDialog.cpp \
    TimeStamp.cpp \
    commands/Command.cpp \
    commands/CommandManager.cpp \
    commands/DeInitUart.cpp \
    commands/GetAllQueue.cpp \
    commands/GetId.cpp \
    commands/InitUart.cpp \
    commands/SerialIO.cpp \
    model/Fragment.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    MainWindow.h \
    SettingsDialog.h \
    TimeStamp.h \
    commands/Command.h \
    commands/CommandManager.h \
    commands/DeInitUart.h \
    commands/GetAllQueue.h \
    commands/GetId.h \
    commands/InitUart.h \
    commands/SerialIO.h \
    model/Fragment.h

FORMS += \
    MainWindow.ui \
    SettingsDialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

DISTFILES +=
