Vue d'ensemble du fonctionnement de Funq
========================================

Fonctionnement client/serveur
-----------------------------

Funq fonctionne en mode **client/server**, via le protocole TCP:

* le serveur est contenu par l'application à tester
* le client permet de manipuler l'application en effectuant des requêtes

.. _trames-echanges:

Trames d'échanges
-----------------

Les trames échangées contiennent du texte simple, du json.

L'entête de trame contient la taille du message (au format texte), suivi
d'un retour à la ligne **\\n**, suivi du message.

Voici un message valide par exemple::
  
  26\n{"action": "widgets_list"}

Choix d'implémentation - partie serveur
---------------------------------------

La partie serveur est écrite en C++, avec le framework QT (comme les cibles des
applications à tester sont en QT). Il est possible d'injecter du code dans une
application déjà existante via **funq** ou de compiler son application avec **libFunq**
pour intégrer le serveur dans une application.

.. note::
  
  Actuellement, l'injection de code sous Windows est fonctionnelle, mais
  incomplète. Par exemple, tester une application construite en DEBUG nécessite
  libFunq compilé et installé en DEBUG. l'exécutable **funq** (serveur) pourrait
  prendre automatiquement la décision si l'installation fournissait les versions DEBUG
  et RELEASE (TODO).
  
  Aussi, le script server/setup.py ne gère pour l'instant que la compil avec
  mingw-32. (TODO)

Choix d'implémentation - partie client
--------------------------------------

Le client est implémenté en Python - et c'est ainsi que les tests doivent être écrits.

Pour lancer les tests, la librairie **nosetests** est utilisée - c'est une
dépendance du client.

Tests unitaires
---------------

Les tests unitaires sont **très importants pour assurer qu'une correction ou**
**un ajout de fonctionnalité n'entrainent pas de régression et que le système**
**continue de bien fonctionner**.

Il est donc indispensable de les **relancer après chaque modification**, **et de les enrichir**.

Le Jenkins de SCLE possède normalement des builds pour cela. La procédure manuelle
est indiquée ci-dessous.

Pour la partie serveur:

.. code-block:: bash
  
  cd server/tests
  qmake && make && make check

.. note::
  
  il est aussi possible de lancer les tests avec couverture de code (Linux):
  
  .. code-block:: bash
    
    cd server
    ./run_tests_lcov.sh && firefox test-lcov-html/index.html

Pour la partie client:

.. code-block:: bash
  
  cd client
  nosetests

.. note::
  
  Pour lancer avec la couverture lancer:
  
  .. code-block:: bash
    
    cd client
    nosetests --with-coverage --cover-package funq

La partie client requiert le framework Python **nose** [1] pour lancer les tests,
et **coverage.py** [2] pour la couverture.

* [1] https://nose.readthedocs.org/en/latest/
* [2] http://nedbatchelder.com/code/coverage/

Documentation
-------------

La documentation joue aussi une part très importante puisqu'elle permet **aux**
**personnes de pouvoir utiliser les outils de manière autonome**.

C'est surtout nécessaire pour la documentation client, **qui est l'aide pour les**
**personnes qui écrivent du code de test basé sur ce framework**.

Toute la documentation du projet est écrite en rst [1] avec l'outil sphinx [2].
Pour générer ce genre de documentation, il faut se placer dans le dossier de doc
(contenant un Makefile, make.bat et index.rst au minimum) et taper la commande
suivante (exemple sous GNU/Linux pour du html):

.. code-block:: bash
  
  make html
  firefox _build/html/index.html

.. note::
  
  Il est très important de **compléter** la documentation dès qu'elle devient obsolète
  ou qu'il y a des ajouts de fonctionnalité.

* [1] http://docutils.sourceforge.net/rst.html
* [2] http://sphinx-doc.org/
