Architecture du serveur Funq
============================

Les sources du serveur sont placées dans le dossier cpp.

Arborescence des fichiers:
--------------------------

* **cpp/funq.pro**: Fichier de construction général qmake
* **cpp/player_tester/**: application exemple, pour tester manuellement le framework
* **cpp/tests/**: dossier de tests unitaires
* **cpp/protocole/**: répertoire pour la couche protocolaire
* **cpp/libFunq/**: répertoire pour la librairie libFunq
* **cpp/funq/**: répertoire pour l'exécutable funq (injection de code libFunq dans une appli)

Ajout de fonctionnalité (commande possible depuis un client)
------------------------------------------------------------

Le fichier le plus important est le fichier **cpp/libFunq/player.h**, avec la définition
de la classe **Player**.

Les slots publics définis à l'intérieur de cette classe seront automatiquement appelés
par un **client qui envoie une commande de même nom que le slot**.

Contenu du fichier *cpp/libFunq/player.h*:

.. literalinclude:: ../cpp/libFunq/player.h
  :language: cpp
