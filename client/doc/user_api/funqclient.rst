Entry point to communicate with a libFunq server : FunqClient
=============================================================

.. currentmodule:: funq.client

A :class:`FunqClient` instance is generally retrieved with
:attr:`funq.testcase.FunqTestcase.funq` or
:attr:`funq.testcase.MultiFunqTestcase.funq`.

Example::

  from funq.testcase import FunqTestCase

  class MyTestCase(FunqTestCase):
      __app_config_name__ = 'my_conf'

      def test_something(self):
          """Method documentation"""

          my_widget = self.funq.widget('mon_widget')

          my_widget.click()

          self.funq.take_screenshot()

.. autoclass:: FunqClient

  .. automethod:: FunqClient.widget

  .. automethod:: FunqClient.active_widget

  .. automethod:: FunqClient.widgets_list

  .. automethod:: FunqClient.dump_widgets_list

  .. automethod:: FunqClient.take_screenshot

  .. automethod:: FunqClient.keyclick

  .. automethod:: FunqClient.shortcut

  .. automethod:: FunqClient.drag_n_drop

  .. automethod:: FunqClient.duplicate
