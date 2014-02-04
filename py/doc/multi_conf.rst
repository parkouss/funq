Plusieurs applications de tests en même temps
=============================================

Il est possible de gérer plusieurs applications de tests en même temps.
Pour cela, il faut déclarer plusieurs sections dans le fichier de conf
**scletest.conf**.

.. code-block:: ini
  
  [applitest]
  executable = ../player_tester
  aliases = applitest.aliases
  hooq_port = 10000

  [applitest2]
  executable = /path/to/my/program
  hooq_port = 10001

.. important::
  
  Attention à spécifier des ports différents pour chaque application !

Maintenant, voyons le code de test::
  
  from scletest.noseplugin import multi_config
  
  # recuperation de la config multiple adéquate
  cfg = multi_config(('applitest', 'applitest2'))
  
  @cfg.with_hooq
  def test_mon_premier_test(hooq_applitest, hooq_applitest2):
      # deux objets hooqs sont passés:
      # - hooq_applitest pour contrôler l'appli "applitest"
      # - hooq_applitest2 pour contrôler l'appli "applitest2"
      pass

.. note::
  
  Il faut remarquer deux différences par rapport au contrôle d'une
  seule application::
  
   - utilisation de **multi_config** au lieu de **config**
   - la fonction de test prends autant de paramètres que le nombre de
     configs déclarées dans l'appel de **multi_config**.

.. note::
  
  Le nombre d'applications testables en même temps n'est pas limité,
  il suffit de les déclarer lors de l'appel de **multi_config** et
  de rajouter le nombre de paramètres adéquat dans les fonctions de test
  décorées.
