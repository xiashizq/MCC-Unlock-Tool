QT       += core gui svg
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Customdialog.cpp \
    backgroundwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    steampathfinder.cpp \
    translationmanager.cpp

HEADERS += \
    Customdialog.h \
    backgroundwidget.h \
    mainwindow.h \
    steampathfinder.h \
    translationmanager.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    translations/halounlocktool_zh_CN.ts

RESOURCES += \
    imgfile.qrc \
    translations/i18n.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    rcfile/icoimg.rc \
    rcfile/imgico.ico \
    README.md

RC_FILE += rcfile/icoimg.rc

VERSION = 1.3.0
QMAKE_TARGET_PRODUCT = Halo Unlock Tool
QMAKE_TARGET_DESCRIPTION = Halo MCC armor and nameplate unlock tool
QMAKE_TARGET_COPYRIGHT = Mumian Youji
