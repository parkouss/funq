Tutorial
========

Cette section a pour but de montrer par l'exemple comment mettre en
place des tests **scleHooq** sur un projet.

Prérequis
---------

Ce tutoriel s'appuie sur l'application de test livrée disponible
à côté de **scleHooq**, **player_tester**.

Cette application doit être compilée pour obtenir un exécutable
nommé **player_tester**.

Le package python **scletest** doit être installé. Pour vérifier, la
commande suivante doit afficher une ligne:

.. code-block:: bash
  
  nosetests -h | grep 'with-scle'

Création de l'arborescence des tests
------------------------------------

L'arborescence d'un projet **scletest** est basiquement un répertoire
contenant des fichiers test\*.py, ainsi qu'un fichier de configuration
nommé **scletest.conf**.

Créer un dossier *tutorial* à côté de l'exécutable **player_tester** et
se déplacer dedans.

.. code-block:: bash
  
  mkdir tutorial
  cd tutorial

Maintenant il faut écrire le fichier de configuration, **scletest.conf**.
Voici un fichier de configuration minimaliste:

.. literalinclude:: tutorial_scletest.conf
  :language: ini
  

Créer un premier test
---------------------

L'écriture d'un test est maintenant possible. Placer le contenu suivant
dans un fichier nommé test_1.py:

.. literalinclude:: tutorial_test_1.py
  :language: python

Ce fichier contient un seul test, qui ne fait rien sauf attendre trois
secondes.

.. note::
  
  la configuration "applitest" est décrite dans le fichier de conf par
  la section du même nom.

.. note::
  
  Le décorateur de fonction **@cfg.with_hooq** permet d'assurer
  le lancement de l'application au début de la fonction et sa fermeture
  à la fin de la fonction.

Exécution des tests
-------------------

Bien ! Exécutons ce premier test. Lancer la commande suivante:

.. code-block:: bash
  
  nosetests --with-scle

Une fenêtre devrait apparaître, et au bout de quelques secondes elle va
se fermer. La sortie de la commande doit ressembler à cela::
  
  .
  ----------------------------------------------------------------------
  Ran 1 test in 4.012s
  
  OK

.. note::
  
  L'option **--with-scle** passée à nosetests indique d'utiliser le
  plugin nommé *scle* qui va lire la configuration avant d'exécuter les
  tests.

.. note::
  
  L'outil **nosetests** dispose de multiples options permettant par exemple
  l'export au format xunit des résultats de test. Voir **nosetests -h**.

Et voilà pour le premier test ! Maintenant complexifions l'exemple avec
l'ajout de deux tests et l'utilisation d'un fichier d'alias.

Tout d'abord, créons le fichier d'alias.

Fichier d'alias
---------------

Un tel fichier associe un nom arbitraire à des objets graphiques
identifiés par leur arborescence (objectName() des objets QT). 
Cette fonctionnalité permet de conserver les tests écrits facilement
lors de déplacement de widgets ou de refactoring de code en permettant
de modifier les chemins qui ont bougé à un seul endroit.

Fichier **applitest.aliases**:

.. literalinclude:: tutorial_aliases

.. note::
  
  Le fichier supporte la substitution de variables par l'utilisation des
  accolades, ce qui permet d'éviter le copié/collé...

Il reste à modifier le fichier de configuration **scletest.conf** pour
indiquer l'emplacement du fichier d'alias. Rajouter la ligne suivante
dans la section **applitest**::
  
  aliases = applitest.aliases

Intérargir avec des widgets
---------------------------

Ecrivons un deuxième fichier de test, **test_widgets.py**:

.. literalinclude:: tutorial_test_widgets.py
  :language: python

On remarque que la variable **hooq** passée dans les fonctions est le
point d'entrée de l'API pour communiquer avec l'application testée.
C'est un objet dont la classe est :class:`scletest.sclehooq.ScleHooqClient`.

Et voilà! Le fichier est assez explicite. Relancer les tests:

.. code-block:: bash
  
  nosetests --with-scle

.. note::
  
  Avec la commande suivante, 3 tests sont lancés! Et oui, c'est normal car
  l'on a écrit 3 tests, répartis sur deux fichiers.
  
  pour lancer les tests d'un seul fichier, utiliser la commande:
  
  .. code-block:: bash
    
    nosetests --with-scle test_widgets.py

.. important::
  
  Il est important même pour du test fonctionnel d'écrire des tests qui ne
  nécessitent pas d'autres tests lancés au préalables - en d'autres termes,
  *l'ordre d'exécution des tests ne doit pas être important*. Ceci permet
  de limiter les effets de bords et ainsi de cibler plus rapidement la
  source d'un bug lors d'échec des tests. Par ailleurs, l'ordre d'exécution
  des tests **nosetests** n'est pas assuré (actuellement c'est le nom
  des fonctions qui influence l'ordre).

Aller plus loin
---------------

Voilà, le tutorial arrive à sa fin. Pour aller plus loin, il faut regarder
la documentation de l'API!
