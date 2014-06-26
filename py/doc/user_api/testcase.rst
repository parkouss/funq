TestCases et écritures de tests
===============================

.. currentmodule:: funq.testcase

TestCases
---------

Les classe de test héritent de :class:`unittest.TestCase` et sont le fondement
pour l'écriture des tests avec **funq**.

.. inheritance-diagram:: BaseTestCase

.. autoclass:: BaseTestCase

.. inheritance-diagram:: FunqTestCase

.. autoclass:: FunqTestCase

.. inheritance-diagram:: MultiFunqTestCase

.. autoclass:: MultiFunqTestCase

Helpers divers
---------------

.. autofunction:: todo

.. autofunction:: parameterized

.. autofunction:: with_parameters

.. autoclass:: MetaParameterized

.. autoexception:: AssertionSuccessError  
