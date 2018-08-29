QT += core gui widgets
TEMPLATE = app
TARGET = Week1
INCLUDEPATH += .
DEFINES += QT_DEPRECATED_WARNINGS
SOURCES += main.cpp mainwindow.cpp chip.cpp algo.cpp worker.cpp
HEADERS += mainwindow.h chip.h worker.h
RESOURCES += resource.qrc
FORMS += mainwindow.ui
QMAKE_CXXFLAGS += -fsanitize=address
QMAKE_LFLAGS += -fsanitize=address
TRANSLATIONS = translations/en.ts translations/zh.ts