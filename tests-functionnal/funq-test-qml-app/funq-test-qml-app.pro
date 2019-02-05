TEMPLATE = app
TARGET = funq-test-qml-app
RESOURCES = resources.qrc
DESTDIR = ./

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets quick qml

CONFIG -= app_bundle

SOURCES += \
    main.cpp
