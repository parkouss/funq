TEMPLATE = lib
TARGET = Funq

DESTDIR     = ../bin

isEmpty(PREFIX) { PREFIX = /usr/local }
target.path = $$PREFIX/bin/
INSTALLS += target

include(../protocole/protocole.pri)

SOURCES += player.cpp \
    funq.cpp \
    objectpath.cpp \
    dragndropresponse.cpp \
    shortcutresponse.cpp \
    pick.cpp

HEADERS += player.h \
    funq.h \
    objectpath.h \
    dragndropresponse.h \
    shortcutresponse.h \
    pick.h

win32 {

HEADERS += \
    WindowsInjector.h

SOURCES += \
    windowsInjector.cpp

} else {

SOURCES += \
    ldPreloadInjector.cpp

}
