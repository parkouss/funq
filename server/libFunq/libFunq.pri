include(../protocole/protocole.pri)

QT += widgets quick testlib

INCLUDEPATH += $${PWD}

SOURCES += $${PWD}/player.cpp \
    $${PWD}/funq.cpp \
    $${PWD}/objectpath.cpp \
    $${PWD}/dragndropresponse.cpp \
    $${PWD}/shortcutresponse.cpp \
    $${PWD}/pick.cpp

HEADERS += $${PWD}/player.h \
    $${PWD}/funq.h \
    $${PWD}/objectpath.h \
    $${PWD}/dragndropresponse.h \
    $${PWD}/shortcutresponse.h \
    $${PWD}/pick.h

win32 {

HEADERS += \
    $${PWD}/WindowsInjector.h

SOURCES += $${PWD}/WindowsInjector.cpp

} else {

SOURCES += \
    $${PWD}/ldPreloadInjector.cpp

}
