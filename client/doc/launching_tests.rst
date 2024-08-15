Launching tests (nose)
======================

Tests are launched by `pytest <https://pytest.readthedocs.org/en/latest/>`_,
and you have to tell it to use the **funq** plugin.

Manual launching
----------------

Basically::

  pytest --with-funq

The command must be started from the folder containing tests files and
the **funq.conf** configuration file.

.. note::

  There are many options for pytest, and some specifics to the **funq**
  plugin. See ``pytest --help`` for an exhaustive list.

Example::

  # launch tests with all stdout/stderr output and stop on the first error
  pytest --with-funq -s -x

Defining default options
------------------------

Every pytest option may be specified by default in a file named **setup.cfg**.
You can look at the pytest documentation for more informations.

Example:

.. code-block:: ini

  [pytest]
  verbosity=2
  with-funq=1

.. note::

  This configuration is very useful, and allow to type only **pytest**
  on the command line instead of ``pytest --with-funq -vv``. I highly
  recommend this configuration and I will use it in the following
  documentation.

Selecting tests to launch
-------------------------

It s possible to select tests to launch using pytest.

Example::

  # every tests in a given file
  pytest test_export.py

  # every tests of a given class in a test file
  pytest test_export.py:TestExportElectre

  # just one test (one method)
  pytest test_export.py:TestExportElectre.test_export_b6

.. note::

  See the pytest documeation fo more information.

  If the verbosity option is equal to 2, the tests execution will show
  test names with the same format. This means that you can then copy/paste
  a test name to restart it.

.. note::

  It is also easy to write your own `pytest plugins <https://docs.pytest.org/en/latest/how-to/writing_plugins.html#writing-your-own-plugin>`_.
