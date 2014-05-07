TARGET = testLibFunq

TEMPLATE = app
CONFIG += testcase qtestlib

INCLUDEPATH += ../../libFunq

SOURCES = test.cpp

SOURCES += ../../libFunq/objectpath.cpp

HEADERS = ../../libFunq/objectpath.h

include(../common.pri)
