
QT       += core network

INCLUDEPATH += $${PWD}

SOURCES += $${PWD}/protocole.cpp \
    $${PWD}/json.cpp \
    $${PWD}/jsonclient.cpp \
    $${PWD}/delayedresponse.cpp

HEADERS += \
    $${PWD}/protocole.h \
    $${PWD}/json.h \
    $${PWD}/jsonclient.h \
    $${PWD}/delayedresponse.h
