Création de fonctions de tests pour nose
========================================

.. currentmodule:: funq.client

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

Récupération de configurations
------------------------------

.. autofunction:: funq.noseplugin.config

.. autofunction:: funq.noseplugin.multi_config

Classes de configuration
------------------------

.. autoclass:: ApplicationConfig
  
  .. automethod:: ApplicationConfig.with_hooq

.. autoclass:: MultiApplicationConfig
  
  .. automethod:: MultiApplicationConfig.with_hooq
