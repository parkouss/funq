Point d'entrée de communication avec un serveur libFunc : FunqClient
====================================================================

.. currentmodule:: funq.client

Une instance de :class:`FunqClient` est généralement récupérée dans les
fonctions décorées par :func:`ApplicationConfig.with_hooq` ou
:func:`MultiApplicationConfig.with_hooq`.

Exemple::
  
  from funq.noseplugin import config
  
  CFG = config('ma_conf')
  
  @CFG.with_hooq
  def test_qquechose(hooq):
    """Documentation de la fonction"""
    
    mon_widget = hooq.widget('mon_widget')
    
    mon_widget.click()
    
    hooq.take_screenshot()

.. autoclass:: FunqClient
  
  .. automethod:: FunqClient.widget
  
  .. automethod:: FunqClient.active_window
  
  .. automethod:: FunqClient.widgets_list
  
  .. automethod:: FunqClient.dump_widgets_list
  
  .. automethod:: FunqClient.take_screenshot
  
  .. automethod:: FunqClient.keyclick
  
  .. automethod:: FunqClient.shortcut
  
  .. automethod:: FunqClient.drag_n_drop
