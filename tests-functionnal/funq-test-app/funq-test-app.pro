TEMPLATE = app
TARGET = funq-test-app
DESTDIR = ./

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets quick

CONFIG -= app_bundle

SOURCES += \
    main.cpp \

HEADERS += \
    widgets.h \
