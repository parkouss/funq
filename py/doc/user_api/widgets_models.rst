Widgets et autres classes du framework
======================================

.. currentmodule:: funq.models

Classe de base Widget
---------------------

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
  
Manipulation des données du framework Model/View de QT
------------------------------------------------------

Pour manipuler des item provenants de QAbstractTableModel, il faut récupérer
la vue adéquate (QAbstractItemView). L'instance retournée héritera de
:class:`AbstractItemView` et les données du modèle rattaché seront accessibles
via la méthode :meth:`AbstractItemView.model_items`

Exemple::
  
  view = hooq.widget('ma_tableview')
  assert isinstance(view, AbstractItemView)
  
  model_items = view.model_items()
  item = model_items.item_by_named_path(['item1'])
  
  item.dclick()


.. inheritance-diagram:: AbstractItemView

.. autoclass:: AbstractItemView
  
  .. automethod:: AbstractItemView.model_items


.. autoclass:: ModelItems
  
  .. automethod:: ModelItems.item_by_named_path
  
  .. automethod:: ModelItems.row_by_named_path


.. autoclass:: ModelItem
  
  .. automethod:: ModelItem.select
  
  .. automethod:: ModelItem.edit
  
  .. automethod:: ModelItem.click
  
  .. automethod:: ModelItem.dclick
