Plusieurs applications de tests en même temps
=============================================

Il est possible de gérer plusieurs applications de tests en même temps.
Pour cela, il faut déclarer plusieurs sections dans le fichier de conf
**funq.conf**.

.. code-block:: ini
  
  [applitest]
  executable = ../player_tester
  aliases = applitest.aliases
  funq_port = 10000

  [applitest2]
  executable = /path/to/my/program
  funq_port = 10001

.. important::
  
  Attention à spécifier des ports différents pour chaque application !

Maintenant, voyons le code de test::
  
  from funq.testcase import MultiFunqTestCase
  
  class MyTestCase(MultiFunqTestCase):
      # recuperation des configs voulues
      __app_config_names__ = ('applitest', 'applitest2')
  
      def test_mon_premier_test(self):
          # les objets FunqClient seront accessibles avec un dictionnaire:
          # - self.funq['applitest'] pour contrôler l'appli "applitest"
          # - self.funq['applitest2'] pour contrôler l'appli "applitest2"
          pass

.. note::
  
  Il faut remarquer quelques différences par rapport au contrôle d'une
  seule application::
  
   - utilisation de la classe :class:`funq.testcase.MultiFunqTestCase`
   - utilisation de **__app_config_names__** au lieu de **__app_config_name__**
   - **self.funq** devient un dictionnaire dont la clé est le nom de la
     configuration est la valeur l'instance de :class:`funq.client.FunqClient`
     associée.

.. note::
  
  Le nombre d'applications testables en même temps n'est pas limité.
