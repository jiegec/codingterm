QT += core gui widgets network svg
TEMPLATE = app
TARGET = Week2
INCLUDEPATH += .
DEFINES += QT_DEPRECATED_WARNINGS
SOURCES += main.cpp mainwindow.cpp newgamedialog.cpp board.cpp
HEADERS += mainwindow.h newgamedialog.h board.h
RESOURCES += resource.qrc
FORMS += mainwindow.ui newgamedialog.ui
QMAKE_CXXFLAGS += -fsanitize=address
QMAKE_LFLAGS += -fsanitize=address
TRANSLATIONS = translations/en.ts translations/zh.ts
