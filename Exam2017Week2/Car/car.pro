QT += core gui widgets network
TEMPLATE = app
TARGET = Car
INCLUDEPATH += .
DEFINES += QT_DEPRECATED_WARNINGS
SOURCES += main.cpp
HEADERS += 
FORMS +=
QMAKE_CXXFLAGS += -fsanitize=address
QMAKE_LFLAGS += -fsanitize=address
