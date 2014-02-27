.. _gkit-aliases:

Alias prédéfinis selon le kit graphique
=======================================

QT nomme différemment certains objets graphiques selon le gestionnaire
de fenêtre utilisé. Des alias sont prédéfinis pour ne pas avoir à différencier
le fichier aliases en fonction des toolkits.

Le fichier de définitions de ces toolkits est ainsi défini:

.. literalinclude:: ../funq/aliases-gkits.conf
  :language: ini

Chaque section définit un toolkit particulier, **default** étant le toolkit
utilisé par défaut.

Il suffit d'utiliser ces alias dans le fichier d'alias de manière
conventionnelle (entre {}) pour les utiliser.

.. note::
  
  Actuellement le framework ne dispose pas de moyen pour détecter le
  kit graphique - **default** est toujours utilisé.
  
  Pour utiliser un autre kit graphique, il faut le spécifier via l'option
  *funq-gkit*. Exemple:
  
  .. code-block:: bash
    
    nosetests --with-funq --funq-gkit kde
