QML tutorial
============

.. versionadded:: 1.2.0

This tutorial shows how to set up tests for a qml application.

.. note::

  Note that this tutorial works on ubuntu, and I suppose on most Linux distros
  also. Windows has not been tested so far for this.


Requirements
------------

You must have installed funq and funq-server compiled with Qt5. Please refer
to the README.rst file about it.


Create the test folder
----------------------

.. code-block::

  mkdir qmltest
  cd qmltest

In this `qmltest` folder, create a funq.conf file:

.. code-block:: ini

  [applitest]
  executable = /usr/bin/qmlscene
  cwd = .
  args = test.qml

.. important::

  `/usr/bin/qmlscene` is the full path of the binary qmlscene provided by Qt.

  Please adapt this path if it is not the same for you.

Then copy paste the content of our application in the test.qml file:

.. code-block:: qml

  import QtQuick 2.0

  Item {

    width: 320
    height: 480

    Rectangle {
        id: rectangle
        color: "#272822"
        width: 320
        height: 480
        MouseArea {
            anchors.fill: parent
            onClicked: rectangle.color = "#FFFFFF"
        }
    }

  }

.. important::

  At this point, you must be able to run::

    /usr/bin/qmlscene test.qml

  If this is not the case, please check your Qt5 installation.


Create the test file
--------------------

Now this is time to write our test. Let's create the test1.py file, still in
the `qmltest` folder:

.. code-block:: python

  from funq.testcase import FunqTestCase

  class TestOne(FunqTestCase):
      # identify the configuration
      __app_config_name__ = 'applitest'

      def test_click_rectangle_change_color(self):
          quick_view = self.funq.active_widget()
          rect = quick_view.item(id="rectangle")

          self.assertEqual(rect.properties()["color"], "#272822")
          # click on the item
          rect.click()
          # color has changed
          self.assertEqual(rect.properties()["color"], "#ffffff")


Run the test
------------

Just run::

  nosetests --with-funq

In the `qmltest` folder and watch it pass.

Going further
-------------

Please look at the specific api documentation: :ref:`quick-objects-api`
