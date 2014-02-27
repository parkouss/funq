Se rattacher à une application de test déjà lancée
==================================================

Il est possible de **se rattacher à une application déjà lancée** au
lieu de laisser les tests démarrer/terminer l'application.

Pour cela, il suffit dans le fichier de config **funq.conf** de
spécifier "socket://" suivi de l'adresse IP dans le champ "executable"
de la section désirée.

.. code-block:: ini
  
  [applitest]
  executable = socket://localhost
  funq_port = 49000

.. important::
  
  Dans ce cas, le framework de test n'est pas responsable du lancement
  et de l'arrêt de l'application. Elle doit être démarrée par ailleurs,
  avec le serveur libFunq intégré.
