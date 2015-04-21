Tutorial
========

This section aims to show by the example how to setup first **funq** tests
on a project.

Requirements
------------

This tutorial is based on a sample QT application that you can find here:
https://github.com/parkouss/funq/tree/master/server/player_tester.

This sample application must be compiled to an executable binary file,
**player_tester**.

To compile player_tester, just::

  cd player_tester
  qmake
  make

This will create a **player_tester** executable file.

The two packages **funq** and **funq-server** must be installed. You
can check that funq-server is installed by running:

.. code-block:: bash

  funq -h

And that funq is installed with:

.. code-block:: bash

  nosetests -h | grep 'with-funq'

.. important::

  **funq-server** and the tested executable **player_tester** must be compiled
  **with the same Qt version**. You can check it with **ldd** for example.

  If you just installed **funq-server** with pip (or did not specified any
  specific qmake path), then you are probably good.

Tests file tree
---------------

The file tree of a **funq** project is basically a folder that contains
test files and a funq configuration file, **funq.conf**.

First, create a *tutorial* folder next to the **player_tester** binary file:

.. code-block:: bash

  mkdir tutorial
  cd tutorial

Now you have to write the configuration file **funq.conf**.
Here is the most minimalistic configuration file:

.. literalinclude:: tutorial_funq.conf
  :language: ini


Write your first test
---------------------

You can now write your first test. Put the following content in a file
called test_1.py:

.. literalinclude:: tutorial_test_1.py
  :language: python

This file contains one test, that do nothing except wait for 3 seconds.

.. note::

  The "applitest" configuration is described in the funq configuration
  file by the section of the same name.

.. note::

  Subclass :class:`funq.testcase.FunqTestCase` ensure that each test method
  will start the application and close it properly.

Test execution
--------------

Well done ! Let's run this first test. Type the following command:

.. code-block:: bash

  nosetests --with-funq

One window must appear, and close after a few seconds. The output on the
terminal must look like this::

  .
  ----------------------------------------------------------------------
  Ran 1 test in 3.315s

  OK

.. note::

  The option ``--with-funq`` given to nosetests allow to use the funq plugin
  that will read the configuration file and execute your tests.

.. note::

  **nosetests** has multiples options to allow for example the generation
  of an xml file to format tests result. See **nosetests -h**.

And voilà! You have written and launched your first funq test. Now let's
go a bit further by adding two tests and use an aliases file.

Let's first create the aliases file.

Aliases file
------------

This file associate a name (an alias) to graphical objects identified by
their path. This behavior allow to keep running the written tests even if
the widgets moves in the tested application or if the code of the tested
application is refactored.

**applitest.aliases** file:

.. literalinclude:: tutorial_aliases

.. note::

  This file support variables substitution by using brackets, allowing
  to avoid useless and dangerous copy/paste.

.. note::

  Some aliases are predefined. See :ref:`gkit-aliases`.

Now you need to modify the **funq.conf** configuration file to indicate
the use of this aliases file. Add the following line in the **applitest**
section::

  aliases = applitest.aliases

Do something with the widgets
-----------------------------

Let's write another tests in a new file, **test_widgets.py**:

.. literalinclude:: tutorial_test_widgets.py
  :language: python

You can see that the member variable **self.funq** is the entry point
to manipulate the tested application. It is an instance of
:class:`funq.client.FunqClient`.

Now you can start tests again:

.. code-block:: bash

  nosetests --with-funq

.. note::

  This time, 5 tests are launched! It's normal because you have written
  5 tests divided up in 2 files.

  To launch the tests of one file only, name it on the command line:

  .. code-block:: bash

    nosetests --with-funq test_widgets.py

.. important::

  It is really important even for functional tests to not write tests
  that depends on others tests. In other words, the
  *order of test execution must not be important*. This allow to limit
  side effects and to find quickly why a test failed. This being said,
  **nosetests** does not assure any order in test execution.

Going further
-------------

This is the end of this tutorial. To go further, you must look at the
API documentation!
