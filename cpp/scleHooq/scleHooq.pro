TEMPLATE = lib
TARGET = scleHooq

DESTDIR     = ../bin

isEmpty(PREFIX) { PREFIX = /usr/local }
target.path = $$PREFIX/bin/
INSTALLS += target

include(hooq.pri)

win32 {

HEADERS += \
    WindowsInjector.h

SOURCES += \
    windowsInjector.cpp

} else {

SOURCES += \
    ldPreloadInjector.cpp

}
