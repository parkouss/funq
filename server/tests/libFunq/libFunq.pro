TARGET = testLibFunq

TEMPLATE = app
CONFIG += testcase qtestlib

INCLUDEPATH += ../../libFunq

DEFINES += SOURCE_DIR=\\\"$${PWD}/\\\"

SOURCES = test.cpp

include(../common.pri)
include(../../libFunq/libFunq.pri)
