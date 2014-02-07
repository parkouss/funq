TEMPLATE = lib
TARGET = scleHooq

DESTDIR     = ../bin

include(hooq.pri)

win32 {

HEADERS += \
    WindowsInjector.h

SOURCES += \
    windowsInjector.cpp

}
