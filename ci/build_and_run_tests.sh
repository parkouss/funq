#!/usr/bin/env bash

flake8 client/funq server/funq_server
cd client && $PYTHON setup.py develop && cd ..
cd server && $PYTHON setup.py develop && cd ..
cd server/tests && qmake && make -j4 && cd ../..
cd tests-functionnal/funq-test-app && qmake && make -j4 && cd ../..

if [[ "$QT_SELECT" == "5" ]]
  then
cd tests-functionnal/funq-test-qml-app && qmake && make -j4 && cd ../..
fi

cd client && $PYTHON setup.py test; cd ..
make -C server/tests/ check
cd tests-functionnal && nosetests && cd ..
