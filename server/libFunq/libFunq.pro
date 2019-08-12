TEMPLATE = lib
TARGET = Funq

DESTDIR     = ../bin

isEmpty(PREFIX) { PREFIX = /usr/local }
target.path = $$PREFIX/bin/
INSTALLS += target

# Disable deprecation warnings since we want to be backward compatible to Qt4
# and thus have to use some methods which are deprecated in recent Qt versions.
DEFINES += QT_NO_DEPRECATED_WARNINGS=1

include(libFunq.pri)
