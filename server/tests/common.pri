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

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
