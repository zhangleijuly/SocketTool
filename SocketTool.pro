QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/udpgroupwidget.cpp \
    src/udpsocketmonitor.cpp \
    src/udpsocketwidget.cpp \
    src/tcpsocketmonitor.cpp \
    src/tcpsocketwidget.cpp \
    src/tcpservermonitor.cpp \
    src/tcpserverwidget.cpp \
    src/main.cpp \
    src/mymainwindow.cpp

HEADERS += \
    include/mymainwindow.h \
    include/stdafx.h \
    include/tcpserverwidget.h \
    include/tcpservermonitor.h \
    include/tcpsocketmonitor.h \
    include/tcpsocketwidget.h \
    include/udpsocketmonitor.h \
    include/udpsocketwidget.h \
    include/udpgroupwidget.h

INCLUDEPATH += include

TRANSLATIONS = i18n/SocketTool_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
