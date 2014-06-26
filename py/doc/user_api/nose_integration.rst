Création de fonctions de tests
==============================

Les fonctions de tests utilisant le framework funq sont créées facilement
via l'héritage depuis la classe :class:`funq.testcase.FunqTestCase` ou
:class:`funq.testcase.MultiFunqTestCase`.

Ces deux classes héritent de unittest.TestCase, qui dispose de beaucoup de
méthodes pratiques pour les assertions. Voir la doc de `Testcase <https://docs.python.org/2/library/unittest.html#unittest.TestCase>`_ .

Exemple::
  
  from funq.testcase import FunqTestCase, MultiFunqTestCase
  
  class MonTest(FunqTestCase):
      app_config_name = 'ma_conf'
  
      def test_qquechose(self):
         print self.app_config  # configuration de l'exécutable testé
         print self.funq  # client pour l'intéraction avec l'exécutable
  
  class MonTest(MultiFunqTestCase):
      app_config_names = ('ma_conf1', 'ma_conf2')
  
      def test_qquechose(self):
         print self.app_config  # configurations des exécutables testés
         print self.funq  # clients pour l'intéraction avec les exécutables

classes de tests
----------------

.. autoclass:: funq.testcase.FunqTestCase

.. autoclass:: funq.testcase.MultiFunqTestCase

Récupération de configurations
------------------------------

La configuration de chaque application est associée à la variable d'instance
**app_config** de :class:`funq.testcase.FunqTestCase`. Dans le cas de
:class:`funq.testcase.MultiFunqTestCase`, **app_config** est un dictionnaire
dont la clé est le nom de la configuration voulue.

Classe de configuration
-----------------------

.. autoclass:: funq.client.ApplicationConfig
  

