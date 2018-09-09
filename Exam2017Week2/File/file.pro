QT += core gui widgets network
TEMPLATE = app
TARGET = File
INCLUDEPATH += .
DEFINES += QT_DEPRECATED_WARNINGS
SOURCES += main.cpp mainwindow.cpp serverworker.cpp mytcpserver.cpp clientworker.cpp
HEADERS += mainwindow.h serverworker.h mytcpserver.h clientworker.h
FORMS += mainwindow.ui
QMAKE_CXXFLAGS += -fsanitize=address
QMAKE_LFLAGS += -fsanitize=address
