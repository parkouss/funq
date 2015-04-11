Test multiple applications at the same time
===========================================

It is possible to test multiple applications in one test method. For
this you will have to write multiple sections in the **funq.conf**
configuration file.

.. code-block:: ini

  [applitest]
  executable = ../player_tester
  aliases = applitest.aliases
  funq_port = 10000

  [applitest2]
  executable = /path/to/my/program
  funq_port = 10001

.. important::

  Be careful to specify different ports for each application !

Let's see the test code now::

  from funq.testcase import MultiFunqTestCase

  class MyTestCase(MultiFunqTestCase):
      # wanted configurations
      __app_config_names__ = ('applitest', 'applitest2')

      def test_my_first_test(self):
          # FunqClient objects will then be accessible with a dict:
          # - self.funq['applitest'] to interact with "applitest"
          # - self.funq['applitest2'] to interact with "applitest2"
          pass

.. note::

  There is some differences when interacting with multiples applications
  at the same time:

   - using :class:`funq.testcase.MultiFunqTestCase`
   - using **__app_config_names__** instead of **__app_config_name__**
   - **self.funq** becomes a dict where keys are configuration names
     and associated values are instances of :class:`funq.client.FunqClient`.

.. note::

  The number testables applications at the same time is not limited.
