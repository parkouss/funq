Launching tests (nose)
======================

Tests are launched by `nose <https://nose.readthedocs.org/en/latest/>`_,
and you have to tell it to use the **funq** plugin.

Manual launching
----------------

Basically::

  nosetests --with-funq

The command must be started from the folder containing tests files and
the **funq.conf** configuration file.

.. note::

  There are many options for nose, and some specifics to the **funq**
  plugin. See ``nosetests --help`` for an exhaustive list.

Example::

  # launch tests with all stdout/stderr output and stop on the first error
  nosetests --with-funq -s -x

Defining default options
------------------------

Every nose option may be specified by default in a file named **setup.cfg**.
You can look at the nose documentation for more informations.

Example:

.. code-block:: ini

  [nosetests]
  verbosity=2
  with-funq=1

.. note::

  This configuration is very useful, and allow to type only **nosetests**
  on the command line instead of ``nosetests --with-funq -vv``. I highly
  recommend this configuration and I will use it in the following
  documentation.

Selecting tests to launch
-------------------------

It s possible to select tests to launch using nose.

Example::

  # every tests in a given file
  nosetests test_export.py

  # every tests of a given class in a test file
  nosetests test_export.py:TestExportElectre

  # just one test (one method)
  nosetests test_export.py:TestExportElectre.test_export_b6

.. note::

  See the nose documeation fo more information.

  If the verbosity option is equal to 2, the tests execution will show
  test names with the same format. This means that you can then copy/paste
  a test name to restart it.

Going further
-------------

**nose** got plenty of usefuls plugins !

Some are integrated in nose, others are third-party plugins and need a
proper installation.

Som of the interesting nose plugins are listed here:

- **xunit**: format tests output using xunit
- **attributeselector**: select tests given their attributes
- **collect-only**: allow to only list tests without really execute them

See the nose documentation, and google to find others usefuls plugins !

.. note::

  It is also easy to write your own `nose plugins <http://nose.readthedocs.org/en/latest/plugins/writing.html>`_.
