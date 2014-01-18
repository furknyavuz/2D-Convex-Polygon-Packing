#-------------------------------------------------
#
# Project created by QtCreator 2012-10-19T17:58:35
#
#-------------------------------------------------

QT       += core gui

TARGET = Graph
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    fpolygon.cpp \
    polygongenerator.cpp \
    algorithms.cpp \
    settings.cpp \
    issuedialog.cpp \
    strip.cpp

HEADERS  += mainwindow.h \
    fpolygon.h \
    polygongenerator.h \
    algorithms.h \
    settings.h \
    issuedialog.h \
    strip.h

FORMS    += mainwindow.ui \
    polygongenerator.ui \
    settings.ui \
    issuedialog.ui \
    algorithms.ui

RESOURCES += \
    logos.qrc
