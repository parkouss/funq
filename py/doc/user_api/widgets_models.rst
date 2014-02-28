Widgets et autres classes du framework
======================================

.. currentmodule:: funq.models

Classe Widget
-------------

Un widget est récupéré depuis la méthode :meth:`funq.client.FunqClient.widget` .

Exemple::
  
  mon_widget = hooq.widget('mon_widget')

.. autoclass:: Widget
  
  .. automethod:: Widget.properties
  
  .. automethod:: Widget.set_properties
  
  .. automethod:: Widget.set_property
  
  .. automethod:: Widget.wait_for_properties
  
  .. automethod:: Widget.click
  
  .. automethod:: Widget.dclick
  
  .. automethod:: Widget.keyclick
  
  .. automethod:: Widget.shortcut
  
