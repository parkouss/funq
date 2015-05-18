TARGET = testLibFunq

TEMPLATE = app
CONFIG += testcase qtestlib

INCLUDEPATH += ../../libFunq

SOURCES = test.cpp

SOURCES += ../../libFunq/objectpath.cpp \
    ../../libFunq/player_utils.cpp \
    ../../libFunq/player.cpp \
    ../../libFunq/player_commands_itemmodel.cpp \
    ../../libFunq/player_commands_gitems.cpp \
    ../../libFunq/player_commands_quickitems.cpp \
    ../../libFunq/dragndropresponse.cpp \
    ../../libFunq/shortcutresponse.cpp

HEADERS = ../../libFunq/objectpath.h \
    ../../libFunq/player_utils.h \
    ../../libFunq/player.h \
    ../../libFunq/dragndropresponse.h \
    ../../libFunq/shortcutresponse.h

include(../common.pri)
include(../../protocole/protocole.pri)
