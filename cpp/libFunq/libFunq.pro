TEMPLATE = lib
TARGET = Funq

DESTDIR     = ../bin

isEmpty(PREFIX) { PREFIX = /usr/local }
target.path = $$PREFIX/bin/
INSTALLS += target

include(../protocole/protocole.pri)

SOURCES += player.cpp \
    funq.cpp \
    objectpath.cpp

HEADERS += player.h \
    funq.h \
    objectpath.h

win32 {

HEADERS += \
    WindowsInjector.h

SOURCES += \
    windowsInjector.cpp

} else {

SOURCES += \
    ldPreloadInjector.cpp

}
