TEMPLATE = lib
TARGET = injectedHooq
DEPENDPATH += .
INCLUDEPATH += .

win32:DEFINES += HOOQ_INJECTED_EXPORT=__declspec(dllexport)
win32:LIBS += user32.lib

QT += network

# Input
HEADERS += \
        ../common/ObjectHookName.h \
        ../common/Communication.h \
	CanaryEvent.h \
	Event.h \
	Logger.h \
	Marshall.h \
	Player.h \
	XmlPropertyDumper.h \

SOURCES += \
        ../common/ObjectHookName.cpp \
	CanaryEvent.cpp \
	Event.cpp \
	Logger.cpp \
	Marshall.cpp \
	Player.cpp \
	XmlPropertyDumper.cpp \

win32 {
	SOURCES += WinDll.cpp
	INSTALLS += target
}

unix {
	SOURCES += EntryPoint.cpp
	INSTALLS += target 
}
