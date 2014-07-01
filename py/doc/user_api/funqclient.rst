Point d'entrée de communication avec un serveur libFunc : FunqClient
====================================================================

.. currentmodule:: funq.client

Une instance de :class:`FunqClient` est généralement récupérée par les membres de
:attr:`funq.testcase.FunqTestcase.app_config` ou de
:attr:`funq.testcase.MultiFunqTestcase.app_config`

Exemple::
  
  from funq.testcase import FunqTestCase
  
  class MonTestCase(FunqTestCase):
      __app_config_name__ = 'ma_conf'
  
      def test_qquechose(self):
          """Documentation de la fonction"""
    
          mon_widget = self.funq.widget('mon_widget')
    
          mon_widget.click()
    
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
