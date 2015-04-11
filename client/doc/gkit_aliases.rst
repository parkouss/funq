.. _gkit-aliases:

Predifined graphical kit aliases
================================

QT is naming some graphical objects differently given the window manager
used. Some aliases are predefined to point to the same objects for multiple
window managers.

Here is the file content that defines these aliases:

.. literalinclude:: ../funq/aliases-gkits.conf
  :language: ini

Each section defines a particular graphical kit (window manager),
**default** being the kit used by default.

You can use these aliases in the aliases file in a standard way (between {}).

.. note::

  Currently the famework does not identify automatically which window
  manager is used - **default** is always used.

  To use another graphical kit, you have to specify it with the *funq-gkit*
  nose option. Example:

  .. code-block:: bash

    nosetests --with-funq --funq-gkit kde
