#!/usr/bin/env bash

# use multiple cores for building
export MAKEFLAGS="-j8"

# Install dependencies on Linux
if [[ "${TRAVIS_OS_NAME-}" == "linux" ]]
then

  # common
  sudo apt-get update -q
  sudo apt-get install -q build-essential xvfb

  # qt
  if [[ "$QT_SELECT" == "4" ]]
  then
    sudo apt-get install -q libqt4-dev qt4-dev-tools
  else
    # Add corresponding PPA from https://launchpad.net/~beineri
    MAJOR=`echo "$QT_SELECT" | cut -d'.' -f 1`
    MINOR=`echo "$QT_SELECT" | cut -d'.' -f 2`
    PATCH=`echo "$QT_SELECT" | cut -d'.' -f 3`
    PREFIX="" && [[ "$MINOR" -ge 10 ]] && PREFIX="-"
    SEPARATOR="" && [[ "$MINOR" -ge 10 ]] && SEPARATOR="."
    sudo add-apt-repository "ppa:beineri/opt-qt${PREFIX}${MAJOR}${SEPARATOR}${MINOR}${SEPARATOR}${PATCH}-trusty" -y
    sudo apt-get update -q
    sudo apt-get install -q "qt${MAJOR}${MINOR}base" "qt${MAJOR}${MINOR}tools" "qt${MAJOR}${MINOR}declarative"
    source "/opt/qt${MAJOR}${MINOR}/bin/qt${MAJOR}${MINOR}-env.sh"
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
