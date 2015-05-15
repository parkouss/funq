Widgets and other classes to interact with tested application
=============================================================

.. currentmodule:: funq.models

The Object base class
---------------------

.. autoclass:: Object

  .. automethod:: Object.properties

  .. automethod:: Object.set_properties

  .. automethod:: Object.set_property

  .. automethod:: Object.wait_for_properties

  .. automethod:: Object.call_slot


The Widget base class
---------------------

A Widget is often obtained with :meth:`funq.client.FunqClient.widget` .

Example::

  my_widget = self.funq.widget('my_widget')

.. inheritance-diagram:: Widget

.. autoclass:: Widget

  .. automethod:: Widget.click

  .. automethod:: Widget.dclick

  .. automethod:: Widget.keyclick

  .. automethod:: Widget.shortcut

  .. automethod:: Widget.drag_n_drop

  .. automethod:: Widget.activate_focus

  .. automethod:: Widget.close

Interacting with the data of QT Model/View framework
----------------------------------------------------

To interact with items in QAbstractTableModel, it is needed to get the
associated view (QAbstractItemView). The returned instance will be of type
:class:`AbstractItemView` and the data will then be retrievable with
the :meth:`AbstractItemView.model_items` method.

Example::

  view = self.funq.widget('my_tableview')
  assert isinstance(view, AbstractItemView)

  model_items = view.model_items()
  item = model_items.item_by_named_path(['item1'])

  item.dclick()


.. inheritance-diagram:: AbstractItemView

.. autoclass:: AbstractItemView

  .. automethod:: AbstractItemView.model_items

  .. automethod:: AbstractItemView.current_editor


.. inheritance-diagram:: TableView

.. autoclass:: TableView

  .. automethod:: TableView.horizontal_header

  .. automethod:: TableView.vertical_header


.. inheritance-diagram:: TreeView

.. autoclass:: TreeView

  .. automethod:: TreeView.header

.. autoclass:: ModelItems

  .. automethod:: ModelItems.iter

  .. automethod:: ModelItems.item_by_named_path

  .. automethod:: ModelItems.row_by_named_path


.. autoclass:: ModelItem

  .. automethod:: ModelItem.select

  .. automethod:: ModelItem.edit

  .. automethod:: ModelItem.click

  .. automethod:: ModelItem.dclick

  .. automethod:: ModelItem.is_checkable

  .. automethod:: ModelItem.is_checked


.. autoclass:: HeaderView

  .. automethod:: HeaderView.header_texts

  .. automethod:: HeaderView.header_click

Interacting with the data of QT Graphics View framework
-------------------------------------------------------

Handling QGraphicsItems data is quite similar to handling data of
the Models/Views framework.

It requires the associated view (an instance of QGraphicsView). In funq
the widget will be an instance of :class:`GraphicsView` and the data will
be available with the :meth:`GraphicsView.gitems` method.

Example::

  gview = self.funq.widget('my_gview')

  gitems = gview.gitems()

  for item in gitems.iter():
      # do something with item


.. inheritance-diagram:: GraphicsView

.. autoclass:: GraphicsView

  .. automethod:: GraphicsView.gitems

  .. automethod:: GraphicsView.dump_gitems
  
  .. automethod:: GraphicsView.grab


.. autoclass:: GItems

  .. automethod:: GItems.iter

.. autoclass:: GItem

  .. automethod:: GItem.is_qobject

  .. automethod:: GItem.properties

  .. automethod:: GItem.click

  .. automethod:: GItem.dclick

Other widgets
-------------

.. inheritance-diagram:: TabBar

.. autoclass:: TabBar

  .. automethod:: TabBar.tab_texts

  .. automethod:: TabBar.set_current_tab


.. inheritance-diagram:: ComboBox

.. autoclass:: ComboBox

  .. automethod:: ComboBox.model_items

  .. automethod:: ComboBox.set_current_text


.. _quick-objects-api:

Interacting with QtQuick objects
--------------------------------

.. versionadded:: 1.2.0

If funq-server is built uppon Qt5 with QtQuick enabled, you can manipulate
QtQuickItems.

Example: ::

  from funq.models import QuickWindow, QuickItem

  # first get the quick view
  quick_view = self.funq.active_widget()
  assert isinstance(quick_view, QuickWindow)

  # get an item, click on it and print its color property
  my_rect = quick_view.item(id='rect')
  assert isinstance(my_rect, QuickItem)
  my_rect.click()
  print(my_rect.properties()["color"])


.. inheritance-diagram:: QuickWindow

.. autoclass:: QuickWindow

  .. automethod:: QuickWindow.item


.. inheritance-diagram:: QuickItem

.. autoclass:: QuickItem

  .. automethod:: QuickItem.click
