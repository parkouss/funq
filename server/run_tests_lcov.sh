#!/bin/bash

rm -f `find tests -name '*.gcno' -o -name '*.gcda'`
rm -f `find tests -name Makefile`
rm -Rf test-lcov-html
rm -f .coverage.*

qmake tests/unittests.pro CONFIG+=debug COVERAGE=1 -r

make -C tests clean
make -C tests -j4

lcov -c -i -d . -o .coverage.base

make -C tests check

lcov -c -d . -o .coverage.run

lcov -d . -a .coverage.base -a .coverage.run -o .coverage.total

lcov -r .coverage.total '/usr/*' 'tests/*' -o .coverage.usefull

genhtml --no-branch-coverage -o test-lcov-html .coverage.usefull
