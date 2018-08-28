QT += core gui widgets
TEMPLATE = app
TARGET = Week1
INCLUDEPATH += .
DEFINES += QT_DEPRECATED_WARNINGS
SOURCES += main.cpp mainwindow.cpp chip.cpp
HEADERS += mainwindow.h chip.h
FORMS += mainwindow.ui