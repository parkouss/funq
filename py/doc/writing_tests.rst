Ecriture de tests
=================

Nomenclature
------------

 - Les tests doivent être écrits dans des fichiers python correspondant au motif **test\*.py**.
 - Les tests peuvent être placés dans des dossiers correspondant au motif **test\***
 - Les tests doivent être écrits dans des classes héritants de :class:`funq.testcase.FunqTestCase`
   ou :class:`funq.testcase.MultiFunqTestCase`.
 - Les tests doivent être des méthodes dans ces classes, dont le nom correspond au motif **test\***.

Exemple:

*fichier test_something.py*:

.. code-block:: python
  
  from funq.testcase import FunqTestCase
  
  class MyClass(object):
      """
      Une classe standard (non test).
      """
  
  class MyTest(FunqTestCase):
      """
      Une classe de test.
      """
      app_config_name = 'ma_conf'
      
      def test_something(self):
          """
          une méthode de test
          """
      
      def something(self):
          """
          une méthode non considérée comme un test
          """

Utilisation des méthodes **assert\***
-------------------------------------

Dans les méthodes de test, il est très fortement recommandé d'utiliser les méthodes
**assert\*** pour détecter les échecs de test.

La liste complète de ces méthodes est accessible dans la documentation de
:class:`unittest.TestCase`. Voici quelques-unes de ces méthodes:

- :meth:`unittest.TestCase.assertTrue`
- :meth:`unittest.TestCase.assertFalse`
- :meth:`unittest.TestCase.assertEqual`
- :meth:`unittest.TestCase.assertNotEqual`
- :meth:`unittest.TestCase.assertIs`
- :meth:`unittest.TestCase.assertIsNot`
- :meth:`unittest.TestCase.assertIn`
- :meth:`unittest.TestCase.assertNotIn`
- :meth:`unittest.TestCase.assertIsInstance`
- :meth:`unittest.TestCase.assertNotIsInstance`
- :meth:`unittest.TestCase.assertRegexpMatches`
- :meth:`unittest.TestCase.assertRaises`
- :meth:`unittest.TestCase.assertRaisesRegexp`
- ...

Exemple::
  
  from funq.testcase import FunqTestCase
  
  class MyTest(FunqTestCase):
      app_config_name = 'ma_conf'
      
      def test_something(self):
          self.assertEqual(1, 1, "Message d'erreur")

Tests todo ou skippés
---------------------

Il est pratique de ne pas lancer de tests dans certains cas (skip) ou encore
de les déclarer "à finir" (todo). pour cela, il existe des décorateurs pratiques:

- :func:`unittest.skip`, :func:`unittest.skipIf`, :func:`unittest.skipUnless`
- :func:`unittest.expectedFailure`
- :func:`funq.testcase.todo`

Exemple::
  
  from funq.testcase import FunqTestCase, todo
  from unittest import skipIf
  import sys
  
  class MyTest(FunqTestCase):
      app_config_name = 'ma_conf'
      
      @todo("En attente de fonctionnement !")
      def test_something(self):
          self.assertEqual(1, 1, "Message d'erreur")
      
      @skipIf(sys.platform.startswith("win"), "requires Windows")
      def test_other_thing(self):
          ....

Tests paramétrés
----------------

Il existe un moyen inclus dans **funq** de générer des fonctions de test à partir
d'une autre méthode et de paramètres. Cela fonctionne pour les classes de test
héritant de :class:`funq.testcase.BaseTestCase` (:class:`funq.testcase.FunqTestCase`
ou :class:`funq.testcase.MultiFunqTestCase`), et en utilisant les décorateurs adaptés:

- :func:`funq.testcase.parameterized`
- :func:`funq.testcase.with_parameters`

Exemple::
  
  from funq.testcase import FunqTestCase, parameterized, with_parameters
  
  PARAMS = [
      ('1', [1], {}),
      ('2', [2], {}),
  ]
  
  class MyTest(FunqTestCase):
      app_config_name = 'ma_conf'
      
      @parameterized('2', 2)
      @parameterized('3', 3)
      def test_something(self, value):
          self.assertGreater(value, 1)
      
      @with_parameters(PARAMS)
      def test_other_thing(self, value):
          self.assertLess(0, value)
