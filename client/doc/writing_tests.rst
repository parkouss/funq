Writing tests
=============

Nomenclature
------------

 - Tests must be written in python files named **test\*.py**
 - Tests may be placed in folders like **test\***
 - Tests must be written as methods of subclasses of :class:`funq.testcase.FunqTestCase`
   or :class:`funq.testcase.MultiFunqTestCase`
 - Tests methods must be named **test\***.

Example:

*file test_something.py*:

.. code-block:: python

  from funq.testcase import FunqTestCase

  class MyClass(object):
      """
      A standard class (not a test one).
      """

  class MyTest(FunqTestCase):
      """
      A test class.
      """
      __app_config_name__ = 'my_conf'

      def test_something(self):
          """
          A test method.
          """

      def something(self):
          """
          A method that is not a test method.
          """

Using **assert\*** methods
--------------------------

Inside test methods, it is highly recommended to use **assert\*** methods
to detect test failures.

The complete list of these methods is availables in the documentation
for :class:`unittest.TestCase`. Here are some of these methods:

- :meth:`unittest.TestCase.assertTrue`
- :meth:`unittest.TestCase.assertFalse`
- :meth:`unittest.TestCase.assertEqual`
- :meth:`unittest.TestCase.assertNotEqual`
- :meth:`unittest.TestCase.assertIs`
- :meth:`unittest.TestCase.assertIsNot`
- :meth:`unittest.TestCase.assertIn`
- :meth:`unittest.TestCase.assertNotIn`
- :meth:`unittest.TestCase.assertIsInstance`
- :meth:`unittest.TestCase.assertNotIsInstance`
- :meth:`unittest.TestCase.assertRegexpMatches`
- :meth:`unittest.TestCase.assertRaises`
- :meth:`unittest.TestCase.assertRaisesRegexp`
- ...

Example::

  from funq.testcase import FunqTestCase

  class MyTest(FunqTestCase):
      __app_config_name__ = 'my_conf'

      def test_something(self):
          self.assertEqual(1, 1, "Error message")

Skipped or todo tests
---------------------

It is useful to not start tests in some cases, or to mark them "uncomplete"
(todo). For this, there are some decorators:

- :func:`unittest.skip`, :func:`unittest.skipIf`, :func:`unittest.skipUnless`
- :func:`unittest.expectedFailure`
- :func:`funq.testcase.todo`

Example::

  from funq.testcase import FunqTestCase, todo
  from unittest import skipIf
  import sys

  class MyTest(FunqTestCase):
      __app_config_name__ = 'ma_conf'

      @todo("Waiting for this to work !")
      def test_something(self):
          self.assertEqual(1, 2, "Error message")

      @skipIf(sys.platform.startswith("win"), "requires Windows")
      def test_other_thing(self):
          ....

Parameterized tests
-------------------

**Funq** offer a way to generate test methods given a base method and some data.
This works for methods of subclasses of :class:`funq.testcase.BaseTestCase`
(:class:`funq.testcase.FunqTestCase` or :class:`funq.testcase.MultiFunqTestCase`),
and by using appropriate decorators:

- :func:`funq.testcase.parameterized`
- :func:`funq.testcase.with_parameters`

Example::

  from funq.testcase import FunqTestCase, parameterized, with_parameters

  PARAMS = [
      ('1', [1], {}),
      ('2', [2], {}),
  ]

  class MyTest(FunqTestCase):
      __app_config_name__ = 'my_conf'

      @parameterized('2', 2)
      @parameterized('3', 3)
      def test_something(self, value):
          self.assertGreater(value, 1)

      @with_parameters(PARAMS)
      def test_other_thing(self, value):
          self.assertLess(0, value)
