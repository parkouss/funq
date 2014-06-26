# -*- coding: utf-8 -*-
"""
Module pour l'intégration avec le framework nosetests.
"""

from funq.client import ApplicationRegistry
from funq.testcase import FunqTestCase
from funq.screenshoter import ScreenShoter
from funq import tools
from nose.plugins import Plugin
from ConfigParser import ConfigParser
import os, codecs, logging

LOG = logging.getLogger('nose.plugins.funq')

def message_with_sep(message):
    """retourne un message avec un séparateur."""
    sep = '-' * 70
    return (sep, message, sep)

def locate_funq():
    """Tente de localiser l'executable scleHooqAttach"""
    return tools.which('funq')

# création d'un Application registry global
_APP_REGISTRY = ApplicationRegistry()

config = _APP_REGISTRY.config # pylint: disable=C0103
multi_config = _APP_REGISTRY.multi_config # pylint: disable=C0103

class FunqPlugin(Plugin):
    """
    Plugin d'integration avec nosetests.
    """
    name = 'funq'

    def options(self, parser, env=None):
        env = env or os.environ
        super(FunqPlugin, self).options(parser, env=env)
        parser.add_option('--funq-conf',
                          dest='funq_conf',
                          default=env.get('NOSE_FUNQ_CONF') or 'funq.conf',
                          help="Fichier de configuration funq, defaut"
                               " `funq.conf` [NOSE_FUNQ_CONF].")
        parser.add_option('--funq-gkit',
                          dest='funq_gkit',
                          default=env.get('NOSE_FUNQ_GKIT') or 'default',
                          help="Specifie le toolkit graphique utilise."
                               " Permet de definir des alias par defaut"
                               " differents. Defaut: `default"
                               " [NOSE_FUNQ_GKIT]`")
        gkit_file = os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                 'aliases-gkits.conf')
        parser.add_option('--funq-gkit-file',
                          dest='funq_gkit_file',
                          default=env.get('NOSE_FUNQ_GKIT_FILE') or gkit_file,
                          help="Specifie le fichier de description du"
                               " toolkit graphique a utiliser. Defaut:"
                               " `%s` [NOSE_FUNQ_GKIT_FILE]" % gkit_file)
        parser.add_option('--funq-attach-exe',
                          dest='funq_attach_exe',
                          default=env.get('NOSE_FUNQ_ATTACH_EXE')
                                                            or locate_funq(),
                          help="Chemin complet ver l'executable funq."
                               " [NOSE_FUNQ_ATTACH_EXE]")
        parser.add_option('--funq-trace-tests',
                          dest='funq_trace_tests',
                          default=env.get('NOSE_FUNQ_TRACE_TESTS'),
                          help="Un fichier dans lequel les traces de debut"
                               " et fin de chaque test seront ajoutees."
                               " [NOSE_FUNQ_TRACE_TESTS]")
        parser.add_option('--funq-trace-tests-encoding',
                          dest='funq_trace_tests_encoding',
                          default=env.get('NOSE_FUNQ_TRACE_TESTS_ENCODING')
                                    or 'utf-8',
                          help="encodage pour le fichier de l'option"
                               "--funq-trace-tests."
                               " [NOSE_FUNQ_TRACE_TESTS_ENCODING]")
        parser.add_option('--funq-screenshot-folder',
                          dest="funq_screenshot_folder",
                          default=env.get("NOSE_FUNQ_SCREENSHOT_FOLDER")
                                    or os.path.realpath("screenshot-errors"),
                          help="Repertoire de stockage des images en erreur."
                               " Defaut: screenshot-errors."
                               " [NOSE_FUNQ_SCREENSHOT_FOLDER]")
        parser.add_option('--funq-snooze-factor',
                          dest="funq_snooze_factor",
                          default=env.get("NOSE_FUNQ_SNOOZE_FACTOR")
                                    or 1.0,
                          help="Permet d'appliquer un facteur sur tous les"
                               " temps d'attente. Defaut: 1.0."
                               " [NOSE_FUNQ_SNOOZE_FACTOR]")

    def configure(self, options, cfg):
        Plugin.configure(self, options, cfg)
        if not self.enabled:
            return
        conf_file = options.funq_conf = os.path.realpath(options.funq_conf)
        if not os.path.isfile(conf_file):
            raise Exception("Fichier de conf funq manquant: `%s`" % conf_file)
        conf = ConfigParser()
        conf.read([conf_file])
        _APP_REGISTRY.register_from_conf(conf, options)
        self.trace_tests = options.funq_trace_tests # pylint: disable=W0201
        self.trace_tests_encoding = (  # pylint: disable=W0201
                                     options.funq_trace_tests_encoding)
        self.screenshoter = ScreenShoter(options.funq_screenshot_folder)
        tools.SNOOZE_FACTOR = float(options.funq_snooze_factor)

    def beforeTest(self, test): # pylint: disable=C0111,C0103,R0201
        message = u"Démarrage de test `%s`" % test.id()
        lines = message_with_sep(message)
        for line in lines:
            LOG.info(line)
        if self.trace_tests:
            with codecs.open(self.trace_tests, 'a',
                                            self.trace_tests_encoding) as f:
                f.write('\n'.join(lines))
                f.write('\n')


    def afterTest(self, test): # pylint: disable=C0111,C0103,R0201,W0613
        message = u"Fin de test `%s`" % test.id()
        lines = message_with_sep(message)
        for line in lines:
            LOG.info(line)
        if self.trace_tests:
            with codecs.open(self.trace_tests, 'a',
                                            self.trace_tests_encoding) as f:
                f.write('\n'.join(lines))
                f.write('\n')
    
    def describeTest(self, test):
        return u'%s' % test.id()
    
    def prepareTestResult(self, result):
        _addError = result.addError
        _addFailure = result.addFailure
        def addError(test, err):
            if isinstance(test.test, FunqTestCase):
                if test.test.CFG and test.test.CFG.screenshot_on_error:
                    self.screenshoter.take_screenshot(test.test.funq, test.id())
            _addError(test, err)
        def addFailure(test, err):
            if isinstance(test.test, FunqTestCase):
                if test.test.CFG and test.test.CFG.screenshot_on_error:
                    self.screenshoter.take_screenshot(test.test.funq, test.id())
            _addFailure(test, err)
        result.addError = addError
        result.addFailure = addFailure
