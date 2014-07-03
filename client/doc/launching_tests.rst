Lancement de tests (nose)
=========================

Les tests sont lancés par `nose <https://nose.readthedocs.org/en/latest/>`_, auquel il faut
indiquer d'utiliser le plugin **funq**.

Lancement manuel
----------------

Basiquement::
  
  nosetests --with-funq

lancé dans le répertoire contenant les tests et le fichier de configuration
**funq.conf**.

.. note::
  
  Il existe de nombreuses options pour nose, et certaines spécifiques au plugin
  **funq**. Voir la commande **nosetests --help** pour une liste exhaustive.

Exemple::
  
  # lancer les tests avec l'affichage des print et stopper lors de la première
  # erreur
  nosetests --with-funq -s -x

Options de nose par défaut
--------------------------

Toutes les options de nose peuvent être spécifiées par défaut dans un fichier
nommé **setup.cfg**. Voir la documentation de nose à ce sujet.

Exemple:

.. code-block:: ini
  
  [nosetests]
  verbosity=2
  with-funq=1

.. note::
  
  La configuration ci-dessous est recommandée par simplicitée, et le reste
  de cette documentation supposera que les commande nose l'utilisent.

Sélection de tests à lancer
---------------------------

Il est possible de sélectionner les tests lancés lors de l'appel de nose.
Voir la documentation de nose à ce sujet.

Exemple::
  
  # tous les tests d'un fichier
  nosetests test_export.py
  
  # tous les tests d'une classe d'un fichier
  nosetests test_export.py:TestExportElectre
  
  # sélection d'un test en particulier
  nosetests test_export.py:TestExportElectre.test_export_b6

.. note::
  
  Si l'option verbosity de nose vaut 2, les tests affichés lors de l'exécution
  apparaissent sous le même format. Il suffit alors de copier/collé le nom du
  test pour le relancer.

Aller plus loin
---------------

**nose** dispose de nombreux plugins qui peuvent être utiles !

Certains sont intégrés dans nose, d'autres nécessitent une installation.

Parmi les plugins de nose intéressants (et fournis par défaut), on peut lister:

- **xunit**: formatage des résultats au format xunit.
- **attributeselector**: selection des tests selon leurs attributs
- **collect-only**: permet de lister les tests sans les exécuter réellement.

Voir la documentation de nose, et google pour découvrir des plugins intéressants !

.. note::
  
  Il est aussi possible d'écrire facilement des `plugins pour nose <http://nose.readthedocs.org/en/latest/plugins/writing.html>`_.
