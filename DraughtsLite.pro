QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DraughtsLite
TEMPLATE = app

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp\
        logic\controller.cpp \
        network\network.cpp \
        gui\createdialog.cpp \
        utilities\gameconfig.cpp \
        gui\boardwidget.cpp \
        logic\chessboard.cpp \
        logic\chesscell.cpp \
    gui/waitingwidget.cpp \
    logic/move.cpp \
    gui/soundplayer.cpp

HEADERS += \
        mainwindow.h \
        common.h \
        logic\controller.h \
        network\network.h \
        gui\createdialog.h \
    utilities\gameconfig.h \
    gui\boardwidget.h \
    logic\chessboard.h \
    logic/chesscell.h \
    gui/waitingwidget.h \
    logic/move.h \
    gui/soundplayer.h \
    utilities/enumerates.h

FORMS += \
        mainwindow.ui \
        gui\createdialog.ui \

PRECOMPILED_HEADER = common.h

RESOURCES += res.qrc

VERSION = 0.0.0.1
QMAKE_TARGET_COMPANY = "Harry Chen"
QMAKE_TARGET_PRODUCT = "Draughts Lite"
QMAKE_TARGET_DESCRIPTION = "A naive draughts game, client & server all in one"
QMAKE_TARGET_COPYRIGHT = "Copyright Harry Chen 2017. All rights Reserved."
