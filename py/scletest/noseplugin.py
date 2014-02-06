#!/usr/bin/env python
# -*- coding: utf-8 -*-

from scletest.sclehooq import ApplicationRegistry
from nose.plugins import Plugin
from ConfigParser import ConfigParser
import os
import logging

LOG = logging.getLogger('nose.plugins.scle')

def log_with_sep(message):
    sep = '-' * 70
    LOG.info(sep)
    LOG.info(message)
    LOG.info(sep)

def _patch_nose_tools_assert_functions():
    """
    patche les fonctions assert_* de nose.tools pour inclure
    des messages longs dans les message d'assertions.
    
    voir nose.tools.trivial.
    """
    from nose import tools
    import unittest
    import re
    
    caps = re.compile('([A-Z])')

    def pep8(name):
        return caps.sub(lambda m: '_' + m.groups()[0].lower(), name)
    
    class Dummy(unittest.TestCase): # pylint: disable=C0111,R0904
        longMessage = True # c'est ce qui change tout.
        
        def nop(self):
            pass
    _t = Dummy('nop')
    for at in [ at for at in dir(_t)
                if at.startswith('assert') and not '_' in at ]:
        pepd = pep8(at)
        setattr(tools, pepd, getattr(_t, at))

# création d'un Application registry global
_APP_REGISTRY = ApplicationRegistry()

config = _APP_REGISTRY.config
multi_config = _APP_REGISTRY.multi_config

class SclePlugin(Plugin):
    name = 'scle'
    
    def options(self, parser, env=None):
        env = env or os.environ
        super(SclePlugin, self).options(parser, env=env)
        parser.add_option('--scle-conf',
                          dest='scle_conf',
                          default=env.get('NOSE_SCLE_CONF') or 'scletest.conf',
                          help="Fichier de configuration scletest, defaut"
                               " `scletest.conf`.")
        parser.add_option('--scle-gkit',
                          dest='scle_gkit',
                          default=env.get('NOSE_SCLE_GKIT') or 'default',
                          help="Specifie le toolkit graphique utilise."
                               " Permet de definir des alias par defaut"
                               " differents. Defaut: `default`")
        gkit_file = os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                 'aliases-gkits.conf')
        parser.add_option('--scle-gkit-file',
                          dest='scle_gkit_file',
                          default=gkit_file,
                          help="Specifie le fichier de description du"
                               " toolkit graphique a utiliser. Defaut:"
                               " `%s`" % gkit_file)
    
    def configure(self, options, conf):
        Plugin.configure(self, options, config)
        if not self.enabled:
            return
        _patch_nose_tools_assert_functions()
        conf_file = options.scle_conf = os.path.realpath(options.scle_conf)
        if not os.path.isfile(conf_file):
            raise Exception("Fichier de conf scle manquant: `%s`" % conf_file)
        conf = ConfigParser()
        conf.read([conf_file])
        _APP_REGISTRY.register_from_conf(conf, options)

    def beforeTest(self, test): # pylint: disable=C0111,C0103,R0201
        log_with_sep(u"Démarrage de test `%s`" % unicode(test.id(), 'utf-8'))
    
    def afterTest(self, test): # pylint: disable=C0111,C0103,R0201
        log_with_sep(u"Fin de test `%s`" % unicode(test.id(), 'utf-8'))
