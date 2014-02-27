.. _disable-attach:

Précompiler son application avec **libFunq**
=============================================

.. note::
  
  Préférer la méthode standard (utilisation de **funq**).
  L'astuce indiquée sur cette page ne doit être utilisée que si la méthode
  standard pose problème.

**funq** permet de s'intégrer dans une application précompilée
*sans besoin de modifier le code source*, grâce à l'injection de code
dynamique effectuée par **funq**. C'est la méthode préférée
puisqu'elle n'induit aucune modification du code source.

Toutefois, il est possible d'intégrer directement libFunq dans une application,
si le besoin s'en fait sentir. Il faudra alors modifier son projet comme suit::
  
  include(../libFunq/libFunq.pri)

Ensuite, dans le main() du programme:

.. code-block:: cpp
  
  #include "funq.h"
  #include <QApplication>
  
  int main(int argc, char *argv[]) {
      QApplication a(argc, argv);
      
      // activation de libFunq
      Funq::activate(true);
      
      /* ... */
      
      return a.exec();
  }

Il faudra ensuite modifier le fichier de configuration **funq.conf**:

.. code-block:: ini
  
  [mon_appli]
  executable = mon_executable
  
  # desactive l'utilisation de funq
  attach = no

Une fois intégré dans la compilation, libFunq devient une
**faille de sécurité importante** puisqu'il permet la manipulation de
l'application depuis l'extérieur via le serveur TCP intégré.

La variable d'environnement **FUNQ_ACTIVATION** si elle est définie
à 1 lance le serveur TCP au lancement entrainant la faille de sécurité.

Pour outrepasser cette contrainte, il est recommandé d'utiliser des
options de compilation (#define) pour ne pas livrer aux clients un outil
intégrant **libFunq**, et au contraire de n'intégrer **libFunq** que
pour les applications testables qui restent en interne dans l'entreprise.

.. important::
  
  La meilleure alternative étant si possible d'utiliser l'injection dynamique
  de code via l'exécutable **funq**.
