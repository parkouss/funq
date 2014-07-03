TARGET = testProtocole

TEMPLATE = app
CONFIG += testcase qtestlib

SOURCES = test.cpp

include(../common.pri)
include(../../protocole/protocole.pri)
