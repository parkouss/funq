.. _disable-attach:

Précompiler son application avec **scleHooq**
=============================================

.. note::
  
  Préférer la méthode standard (utilisation de **scleHooqAttach**).
  L'astuce indiquée sur cette page ne doit être utilisée que si la méthode
  standard pose problème.

**scleHooq** permet de s'intégrer dans une application précompilée
*sans besoin de modifier le code source*, grâce à l'injection de code
dynamique effectuée par **scleHooqAttach**. C'est la méthode préférée
puisqu'elle n'induit aucune modification du code source.

Toutefois, il est possible d'intégrer directement scleHooq dans une application,
si le besoin s'en fait sentir. Il faudra alors modifier son projet comme suit::
  
  include(../scleHooq/hooq.pri)

Ensuite, dans le main() du programme:

.. code-block:: cpp
  
  #include "scleHook.h"
  #include <QApplication>
  
  int main(int argc, char *argv[]) {
      QApplication a(argc, argv);
      
      // activation de scleHooq
      ScleHooq::activation();
      
      /* ... */
      
      return a.exec();
  }

Il faudra ensuite modifier le fichier de configuration **scletest.conf**:

.. code-block:: ini
  
  [mon_appli]
  executable = mon_executable
  
  # desactive l'utilisation de scleHooqAttach
  scle_attach = no

Une fois intégré dans la compilation, scleHooq devient une
**faille de sécurité importante** puisqu'il permet la manipulation de
l'application depuis l'extérieur via le serveur TCP intégré.

La variable d'environnement **SCLEHOOQ_ACTIVATION** si elle est définie
à 1 lance le serveur TCP au lancement entrainant la faille de sécurité.

Pour outrepasser cette contrainte, il est recommandé d'utiliser des
options de compilation (#define) pour ne pas livrer aux clients un outil
intégrant **scleHooq**, et au contraire de n'intégrer **scleHooq** que
pour les applications testables qui restent en interne dans l'entreprise.

.. important::
  
  La meilleur alternative étant si possible d'utiliser l'injection dynamique
  de code via **scleHooqAttach**.
