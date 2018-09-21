#!/usr/bin/env bash

# use multiple cores for building
export MAKEFLAGS="-j8"

# Install dependencies on Linux
if [[ "${TRAVIS_OS_NAME-}" == "linux" ]]
then

  # common
  sudo apt-get update -qq
  sudo apt-get install -qq build-essential xvfb

  # qt
  if [[ "$QT_SELECT" == "4" ]]
  then
    sudo apt-get install -qq libqt4-dev qt4-dev-tools
  elif [[ "$QT_SELECT" == "5" ]]
  then
    sudo apt-get install -qq qtbase5-dev qtdeclarative5-dev qtdeclarative5-qtquick2-plugin qttools5-dev-tools
  fi

  # python packages
  pip install flake8

  # xvfb
  export DISPLAY=:99.0
  sh -e /etc/init.d/xvfb start


# Install dependencies on OS X
elif [[ "${TRAVIS_OS_NAME-}" == "osx" ]]
then

  # ccache
  brew update
  brew install ccache
  export PATH="/usr/local/opt/ccache/libexec:$PATH"

  # qt
  brew install qt5
  brew link --force qt5

  # python packages
  pip${PYTHON/python/} install --user flake8
  export PATH="$PATH:`$PYTHON -m site --user-base`/bin"

fi
