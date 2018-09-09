QT += core gui widgets network
TEMPLATE = app
TARGET = Transfer
INCLUDEPATH += .
DEFINES += QT_DEPRECATED_WARNINGS
SOURCES += main.cpp mainwindow.cpp board.cpp
HEADERS += mainwindow.h board.h
FORMS += mainwindow.ui
QMAKE_CXXFLAGS += -fsanitize=address
QMAKE_LFLAGS += -fsanitize=address
