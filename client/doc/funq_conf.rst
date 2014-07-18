The *funq.conf* configuration file
==================================

This configuration file describe the execution tests environment.

It's an **ini** configuration file that contains sections (between
[brackets]), and eah section is the configuration of one application
to test.

Here is an example of a funq configuration file:

.. literalinclude:: funq.conf
  :language: ini

The only required option in each section is **executable**. If its value
starts with "socket://" then the application will not be launched but
the tests will try to connect to the address instead. This is the **detached**
mode, allowing to test an already launched application (note that this
application will have to be started with the **funq** executable, or
compiled with libFunq).

Here is the list of the facultative availables options:

 - **args**: executable arguments
 - **funq_port**: libFunq communication port used (défaut: 9999). May be
   0, an in this case the OS will pick the first available port.
 - **cwd**: path to the execution directory. By default, this is the
   executable directory.
 - **aliases**: path to the aliases file.
 - **executable_stdout**: a file to save stdout output. Can be null to not
   redirect output.
 - **executable_stderr**: a file to save stderr output. Can be null to not
   redirect output.
 - **timeout_connection**: timeout in seconds to try to connect with the
   libFunq socket. Defaults to 10 seconds.
 - **attach**: set to "no" or "0" to enable the *detached mode* of funq.
   See :ref:`disable-attach`.
 - **with_valgrind**: set to "1" or "yes" to activate valgrind
 - **valgrind_args**: valgrind arguments. By default,
   "--leak-check=full --show-reachable=yes".
 - **screenshot_on_error**: set to "1" or "yes" to automatically take
   screenshot on tests errors. A *screenshot-errors* will then be created
   and will contains screnshots of failed tests.
