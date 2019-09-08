The *funq* project
==================

**funq** is a tool to write FUNctional tests for Qt applications, both Widgets
and QML, using python.

It is licenced under the CeCILL v2.1 licence (very close to the GPL v2).
See the LICENCE.txt file distributed in the sources for more information.

The licence may appear restrictive, but as **funq** is a testing tool, you
probably just don't mind. Do not deliver funq alongside your code source
or compiled binaries if your licence is not compatible, that's all. You can
still use the sources, or link with funq libraries for a personal use
(like testing!).

Please feel free to contribute to this project by creating github issues,
pull requests, or simply staring the project! It will be greatly appreciated.

Documentation:

.. image:: https://readthedocs.org/projects/funq/badge/?version=latest
    :target: http://funq.readthedocs.org

CI build status:

.. image:: https://travis-ci.org/parkouss/funq.svg?branch=master
    :target: https://travis-ci.org/parkouss/funq

.. image:: https://ci.appveyor.com/api/projects/status/github/parkouss/funq?branch=master&svg=true
    :target: https://ci.appveyor.com/project/parkouss/funq

Get funq on PyPi (server and client packages):

.. image:: https://img.shields.io/pypi/v/funq-server.svg
    :target: https://pypi.python.org/pypi/funq-server/

.. image:: https://img.shields.io/pypi/v/funq.svg
    :target: https://pypi.python.org/pypi/funq/

Examples
========

Run your application like::

  funq --host 0.0.0.0 --port 9000 YourApp

Then you can call from python like this::

  from funq.client import FunqClient

  funq = FunqClient("192.168.0.17", 9000)
  funq.widget('btnTest').click()


Installation
============

You can easily install it from PyPi with pip or setuptools::

  pip install funq-server
  pip install funq

Note that funq-server will need qmake to build the C++ part of the server,
and this installation will be Qt-compatible with the same Qt version of
qmake.

To specify the path to qmake, you can define the **FUNQ_QMAKE_PATH**
environment variable: ::

  FUNQ_QMAKE_PATH=/usr/bin/qmake-qt5 pip install funq-server

Also, if you're not using virtualenv you may have to take root
privileges to install **funq**.

You can instead get the sources and install it::

  cd funq/
  pip install server
  pip install client

For contributors, you may want to use **pip install -e** instead of
**pip install** commands. Note that **virtualenv** is highly recommended,
so you can easily manage multiple python2/python3/Qt4/Qt5 environments.

When installing funq-server from sources, you can create a server/setup.cfg
file to specify the qmake path::

  [build_libfunq]
  qmake_path = /usr/bin/qmake-qt5

Before running the *pip install* command, or use the **FUNQ_QMAKE_PATH**
environment variable.

How does *funq* works
=====================

**funq** is divided in two parts:

- **funq-server** is the server part of the project, composed of an
  executable called **funq** and a dynamic library **libFunq**. The
  **funq** executable allows to inject some code in a Qt application
  to start a TCP server that will allow to interact with the application.
  This is currently not working with Python 3 on Windows, but you can still
  build you application with libFunq as a workaround.

- **funq** is a python package that offers an API to interact with a
  **libFunq** TCP server. It is the client side of the project, and uses
  nosetests to launch FUNctional Qt tests.

Known restrictions
==================

Funq currently works with python >= 2.7 (it is fully compatible with python 3),
Qt4 and Qt5 on GNU/Linux and macOS.

It also works on Windows, but only with Python 2.7 out of the box. With
Python 3, the tested application has to be compiled with libFunq because the
package *winappdbg* (needed for the DLL injection) is not available for Python 3
(any help welcome!).

Documentation
=============
Documentation is available at https://funq.readthedocs.io/

Thanks to
=========

Thanks to Yann De Poulpiquet <yann_de_poulpiquet@bestmail.us> and
Riad Lezzar <rlezzar@gmail.com> to have contributed by writing the firsts
functional tests with **funq**.

Thanks also to Jean-Luc Rouzoul, Dominique Constant and Mickaël Guérin for
having supported this project.

Without them, **funq** would never have become a free software !
