Le fichier de configuration funq.conf
=====================================

Ce fichier de configuration décrit l'environnement d'exécution des tests.

C'est un fichier de configuration au format **ini**, qui contient des
sections (entre [crochets]), et chaque section représente une configuration
d'application à tester.

Voici un exemple de fichier de configuration:

.. literalinclude:: funq.conf
  :language: ini

Seule l'option "executable" est requise pour chaque section
Si la valeur de "executable" commence par "socket://" l'application
ne sera pas démarrée mais le test se raccrochera à la socket
indiquée par l'adresse IP indiquée à la suite. De même, l'application
ne sera pas fermée dans ce cas à la fin des tests. (mode détaché)

Voici la liste des options falcutatives disponibles:

 - **args**: arguments de l'executable de test
 - **funq_port**: port de communication utilisé (défaut: 9999) Si 0, le
   système prendra le premier port libre.
 - **cwd**: chemin indiquant le dossier d'exécution de l'executable.
   Par défaut, c'est le répertoire de l'exécutable
 - **aliases**: chemin indiquent un fichier d'alias
 - **executable_stdout** spécifie un fichier de sortie pour enregistrer le
   flux stdout du programme testé. Si NULL, le flux n'est pas redirigé.
 - **executable_stderr** spécifie un fichier de sortie pour enregistrer le
   flux stderr du programme testé. Si NULL, le flux n'est pas redirigé.
 - **timeout_connection**: temps d'attente maximal en seconde pour tenter
   de se connecter à la socket libFunq. Défaut 10 secondes.
 - **attach**: positionner à "no" ou "0" pour désactiver l'utilisation
   de funq. Voir :ref:`disable-attach`.
 - **with_valgrind**: positionner à "1" ou "yes" pour activer valgrind
 - **valgrind_args**: arguments pour valgrind. par défaut,
   "--leak-check=full --show-reachable=yes".
 - **screenshot_on_error**: positionner à "1" ou "yes" pour automatiquement
   prendre des screenshots lors d'erreur d'exécution de test. Le répertoire
   *screenshot-errors* sera alors créé et contiendra les images des tests en
   erreur.
