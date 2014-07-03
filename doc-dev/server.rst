Architecture du serveur Funq
============================

Les sources du serveur sont placées dans le dossier cpp.

Arborescence des fichiers:
--------------------------

* **server/funq.pro**: Fichier de construction général qmake
* **server/player_tester/**: application exemple, pour tester manuellement le framework
* **server/tests/**: dossier de tests unitaires
* **server/protocole/**: répertoire pour la couche protocolaire
* **server/libFunq/**: répertoire pour la librairie libFunq
* **server/funq/**: répertoire pour l'exécutable funq (injection de code libFunq dans une appli)

Ajout de fonctionnalités (commandes possibles depuis un client)
---------------------------------------------------------------

Le fichier le plus important est le fichier **server/libFunq/player.h**, avec la définition
de la classe **Player**.

Les slots publics définis à l'intérieur de cette classe seront automatiquement appelés
par un **client qui envoie une commande de même nom que le slot**.

Une **commande** client est un objet json (le format brut est défini ici:
:ref:`trames-echanges`) qui possède au moins une clé nommée **"action"**
dont **la valeur est le nom de la commande**.

Par exemple, voici l'implémentation de la commande **quit** (qui permet de quitter
la QApplication), coté c++:

player.h:

.. code-block:: cpp
  
  public slots:
    ...
    QtJson::JsonObject quit(const QtJson::JsonObject & command);

player.cpp:

.. code-block:: cpp
  
  QtJson::JsonObject Player::quit(const QtJson::JsonObject &) {
      if (qApp) {
          qApp->quit();
      }
      QtJson::JsonObject result;
      return result;
  }

pour déclencher cette commande côté client, on devra donc envoyer par le réseau
l'objet json suivant, (ici sans l'entête de trame définie ici :ref:`trames-echanges`):

.. code-block:: python
  
  {"action": "quit"}

Et la réponse du serveur sera un objet json vide.

.. note::
  
  Il faut bien retenir que côté serveur, l'ajout d'un slot avec la bonne signature
  rajoute une commande, simplement.

.. note::
  
  Les fichier **server/libFunq/player.h** et **server/libFunq/player.cpp** sont une bonne
  base pour apprendre comment ajouter des commandes et les implémenter.

Contenu du fichier *server/libFunq/player.h*
--------------------------------------------

Voici le contenu du fichier player.h. Pour bien comprendre, regarder les sources
complètes dans **server/libFunq**.

.. literalinclude:: ../server/libFunq/player.h
  :language: cpp
