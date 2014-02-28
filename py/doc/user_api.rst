API pour les utilisateurs du framework funq
===========================================

Création de fonctions de tests pour nose
----------------------------------------

Les fonctions de tests utilisant le framework funq sont créées facilement
via un décorateur *with_hooq*. Ce décorateur est accessible depuis une instance
de :class:`funq.client.ApplicationConfig` ou :class:`funq.client.MultiApplicationConfig`.

De telles instances sont créées automatiquement par le framework lors de la lecture
du fichier de conf *funq.conf* et récupérées par les fonctions correspondantes
:func:`funq.noseplugin.config` et :func:`funq.noseplugin.multi_config`.

Exemple::
  
  from funq.noseplugin import config
  
  CFG = config('ma_conf')
  
  @CFG.with_hooq
  def test_qquechose(hooq):
     ...

.. currentmodule:: funq.client

Récupération de configurations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: funq.noseplugin.config

.. autofunction:: funq.noseplugin.multi_config

Classes de configuration
~~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: ApplicationConfig
  
  .. automethod:: ApplicationConfig.with_hooq

.. autoclass:: MultiApplicationConfig
  
  .. automethod:: MultiApplicationConfig.with_hooq

Point d'entrée de communication avec un serveur libFunc : FunqClient
--------------------------------------------------------------------

Une instance de :class:`FunqClient` est généralement récupérée dans les
fonctions décorées par :func:`ApplicationConfig.with_hooq` ou
:func:`MultiApplicationConfig.with_hooq`.

.. autoclass:: FunqClient
  
  .. automethod:: FunqClient.widget
  
  .. automethod:: FunqClient.widgets_list
  
  .. automethod:: FunqClient.dump_widgets_list
  
  .. automethod:: FunqClient.take_screenshot
  
  .. automethod:: FunqClient.keyclick
  
  .. automethod:: FunqClient.shortcut
