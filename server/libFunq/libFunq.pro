TEMPLATE = lib
TARGET = Funq

DESTDIR     = ../bin

isEmpty(PREFIX) { PREFIX = /usr/local }
target.path = $$PREFIX/bin/
INSTALLS += target

include(libFunq.pri)
