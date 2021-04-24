QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 c++17

QMAKE_CXXFLAGS += -Wno-unused-function -Wno-unused-parameter

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    alert.cpp \
    apps.cpp \
    brightnessdialog.cpp \
    calendarapp.cpp \
    generaldialog.cpp \
    main.cpp \
    mainwindow.cpp \
    quit.cpp \
    reader.cpp \
    savedwords.cpp \
    settings.cpp \
    settingschooser.cpp \
    usbms_splash.cpp

HEADERS += \
    alert.h \
    apps.h \
    brightnessdialog.h \
    calendarapp.h \
    functions.h \
    generaldialog.h \
    mainwindow.h \
    quit.h \
    reader.h \
    savedwords.h \
    settings.h \
    settingschooser.h \
    usbms_splash.h

FORMS += \
    alert.ui \
    apps.ui \
    brightnessdialog.ui \
    calendarapp.ui \
    generaldialog.ui \
    mainwindow.ui \
    quit.ui \
    reader.ui \
    savedwords.ui \
    settings.ui \
    settingschooser.ui \
    usbms_splash.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    eink.qrc
