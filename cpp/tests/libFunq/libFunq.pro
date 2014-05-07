TARGET = testLibFunq

TEMPLATE = app
CONFIG += testcase qtestlib

INCLUDEPATH += ../../libFunq

SOURCES = test.cpp

SOURCES += ../../libFunq/objectpath.cpp ../../libFunq/player.cpp ../../libFunq/dragndropresponse.cpp

HEADERS = ../../libFunq/objectpath.h ../../libFunq/player.h ../../libFunq/dragndropresponse.h

include(../common.pri)
include(../../protocole/protocole.pri)
