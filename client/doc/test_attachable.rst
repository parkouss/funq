Attach to an already started application
========================================

It is possible to **attach to an already started application** instead
of letting funq start and shutdown the application.

For this you need to specify in the **funq.conf** configuration file
"socket://" followed by the IP address in the "executable" field.

.. code-block:: ini
  
  [applitest]
  executable = socket://localhost
  funq_port = 49000

.. important::
  
  In this case, funq is not responsible for starting and stopping the
  tested application. This must be done somewhere else, with the libFunq
  server integrated.
