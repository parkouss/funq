
QT += core gui network

INCLUDEPATH += $${PWD}
DEPENDPATH += $${PWD}

SOURCES += $${PWD}/ObjectHookName.cpp \
        $${PWD}/Player.cpp \
        $${PWD}/Event.cpp \
        $${PWD}/scleHook.cpp

HEADERS  += \
        $${PWD}/ObjectHookName.h \
        $${PWD}/Player.h \
        $${PWD}/Event.h \
        $${PWD}/scleHook.h
