The *funq* project
==================

**funq** is a tool to write FUNctional tests for Qt applications
using python.

It is licenced under the CeCILL v2.1 licence (very close to the GPL v2).
See the LICENCE.txt file distributed in the sources for more information.

How does *funq* works
=====================

**funq** is divided in two parts:

- **funq-server** is the server part of the project, composed of an
  executable called **funq** and a dynamic library **libFunq**. The
  **funq** executable allows to inject some code in a QT application
  to start a TCP server that will allow to interact with the application.

- **funq** is a python package that offers an API to interact with a
  **libFunq** TCP server. It is the client side of the project, and uses
  nosetests to launch FUNctional Qt tests.

Known restrictions
==================

Funq currently only works with python2.7, QT4, and on GNU/Linux.

It may work under Windows XP and some other versions but this has
not been really tested.

There are plans to make it work under windows, support python 3
and also support QT5. Contributors are welcome to help in these tasks !

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

Getting started and documentation
=================================

Look at the documentation on readthedocs: http://funq.readthedocs.org/en/latest/
