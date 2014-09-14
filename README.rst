The *funq* project
==================

**funq** is a tool to write FUNctional tests for Qt applications
using python.

It is licenced under the CeCILL v2.1 licence (very close to the GPL v2).
See the LICENCE.txt file distributed in the sources for more information.

Tutorial and documentation:

.. image:: https://readthedocs.org/projects/funq/badge/?version=latest
    :target: http://funq.readthedocs.org

Travis-ci build:

.. image:: https://travis-ci.org/parkouss/funq.svg?branch=master
    :target: https://travis-ci.org/parkouss/funq

Get funq on PyPi (server and client packages):

.. image:: https://pypip.in/version/funq-server/badge.png
    :target: https://pypi.python.org/pypi/funq-server/

.. image:: https://pypip.in/version/funq/badge.png
    :target: https://pypi.python.org/pypi/funq/

How does *funq* works
=====================

**funq** is divided in two parts:

- **funq-server** is the server part of the project, composed of an
  executable called **funq** and a dynamic library **libFunq**. The
  **funq** executable allows to inject some code in a Qt application
  to start a TCP server that will allow to interact with the application.

- **funq** is a python package that offers an API to interact with a
  **libFunq** TCP server. It is the client side of the project, and uses
  nosetests to launch FUNctional Qt tests.

Known restrictions
==================

Funq currently only works with python >= 2.7, Qt4 (Qt5 support is
experimental but seems to work pretty well), and on GNU/Linux.

The server is currently not compatible with python3, but the client is
and so tests may be written in python 3.

Funq may work under Windows XP and some other versions but this has
not been really tested.

There are plans to make it work under windows and also fully support Qt5.
Contributors are welcome to help in these tasks !

Installation
============

You can easily install it from PyPi with pip or setuptools::
  
  pip install funq-server
  pip install funq

.. note::
  
  Note that funq-server will need qmake-qt4 to build the C++ part
  of the server.
  
  Also, if you're not using virtualenv you may have to take root
  privileges to install **funq**.

You can instead get the sources and install it with setup.py::
  
  cd server
  python setup.py install
  
  cd ../client
  python setup.py install

.. note::
  
  For contributors, you may want to use **python setup.py develop**
  instead of **python setup.py install** commands.

.. note::
  
  To install funq for qt5, first create a server/setup.cfg file with
  the following content::
    
    [build_libfunq]
    qmake_path = /usr/bin/qmake-qt5
  
  before running the *python setup.py install* command.

Thanks to
=========

Thanks to Yann De Poulpiquet <yann_de_poulpiquet@bestmail.us> and
Riad Lezzar <rlezzar@gmail.com> to have contributed by writing the firsts
functional tests with **funq**.

Thanks also to Jean-Luc Rouzoul, Dominique Constant and Mickaël Guérin for
having supported this project.

Without them, **funq** would never have become a free software !
