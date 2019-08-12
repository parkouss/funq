# compile avec les flags pour le coverage
# fonctionne avec clang / gcc sous unix
#
# utilisation: qmake CONFIG+=debug COVERAGE=1
unix {
    contains(COVERAGE, 1) {
        QMAKE_CXXFLAGS_DEBUG += --coverage
        QMAKE_LFLAGS_DEBUG += --coverage
    }
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets quick

# Disable deprecation warnings since we want to be backward compatible to Qt4
# and thus have to use some methods which are deprecated in recent Qt versions.
DEFINES += QT_NO_DEPRECATED_WARNINGS=1
