.. _disable-attach:

Compile the application with **libFunq**
========================================

.. important::

  Always prefer the standard approach (use **funq** executable) when
  possible. This part of the documentation is only useful when the
  standard approach is not working.


**libFunq** can be integrated in an already compiled application with
the code injection done by the **funq** executable. It is the preferred
way since this does not require to modify the source code of the tested
application.

But it is also possible to integrate directly libFunq in an application
if you need it. You will then need to modify your .pro file like this::

  include(/path/to/libFunq/libFunq.pri)

Then, in the main of your program:

.. code-block:: cpp

  #include "funq.h"
  #include <QApplication>

  int main(int argc, char *argv[]) {
      QApplication a(argc, argv);

      // libFunq activation
      Funq::activate(true);

      /* ... */

      return a.exec();
  }

You will then need to adapt the **funq.conf** configuration file:

.. code-block:: ini

  [my_application]
  executable = mon_executable

  # does not use funq executable to inject libFunq
  attach = no

Once integrated in the compiled application, libFunq becomes a **security hole**
as it allows to interact with the application by using the integrated TCP
server.

The environment variable **FUNQ_ACTIVATION** if defined to 1 starts the
TCP server at applcation startup and will allow funq clients to interact
with the application.

To bypass this constraint, it is recommended to use #define in your code
to integrate libFunq only for testing purpose and not deliver to final users
an application with funq included.

.. important::

  The best alternative is to use the dynamic injection provided by the
  executable **funq** when possible.
