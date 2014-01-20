A propos de scleHooq
====================

Une fois intégré dans la compilation, scleHooq devient une
**faille de sécurité importante** puisqu'il permet la manipulation de
l'application depuis l'extérieur via le serveur TCP intégré.

La variable d'environnement **SCLEHOOQ_ACTIVATION** si elle est définie
à 1 lance le serveur TCP au lancement entrainant la faille de sécurité.

Pour outrepasser cette contrainte, il est recommandé d'utiliser des
options de compilation (#define) pour ne pas livrer aux clients un outil
intégrant **scleHooq**, et au contraire de n'intégrer **scleHooq** que
pour les applications testables qui restent en interne dans l'entreprise.
