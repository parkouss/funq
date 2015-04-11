# -*- coding: utf-8 -*-

# Copyright: SCLE SFE
# Contributor: Julien Pagès <j.parkouss@gmail.com>
#
# This software is a computer program whose purpose is to test graphical
# applications written with the QT framework (http://qt.digia.com/).
#
# This software is governed by the CeCILL v2.1 license under French law and
# abiding by the rules of distribution of free software.  You can  use,
# modify and/ or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL v2.1 license and that you accept its terms.

"""
Module that integrates funq with nosetests.
"""

from funq.client import ApplicationRegistry
from funq.testcase import MultiFunqTestCase, FunqTestCase, \
    register_funq_app_registry
from funq.screenshoter import ScreenShoter
from funq import tools
from nose.plugins import Plugin
from ConfigParser import ConfigParser
import os
import codecs
import logging

LOG = logging.getLogger('nose.plugins.funq')


def message_with_sep(message):
    """Returns a message with a separator."""
    sep = '-' * 70
    return (sep, message, sep)


def locate_funq():
    """find the funq executable"""
    return tools.which('funq')


class FunqPlugin(Plugin):

    """
    Nosetests plugin to integrate funq.
    """
    name = 'funq'

    _instance = None

    @classmethod
    def instance(cls):
        return cls._instance

    def options(self, parser, env=None):
        env = env or os.environ
        super(FunqPlugin, self).options(parser, env=env)
        parser.add_option('--funq-conf',
                          dest='funq_conf',
                          default=env.get('NOSE_FUNQ_CONF') or 'funq.conf',
                          help="funq configuration file, defaults to"
                               " `funq.conf` [NOSE_FUNQ_CONF].")
        parser.add_option('--funq-gkit',
                          dest='funq_gkit',
                          default=env.get('NOSE_FUNQ_GKIT') or 'default',
                          help="Choose a specific graphic toolkit. This allows"
                               " to define default different aliases"
                               " Default: `default` [NOSE_FUNQ_GKIT]`")
        gkit_file = os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                 'aliases-gkits.conf')
        parser.add_option('--funq-gkit-file',
                          dest='funq_gkit_file',
                          default=env.get('NOSE_FUNQ_GKIT_FILE') or gkit_file,
                          help="Override the file that defines graphic"
                               " toolkits. Default: `%s` [NOSE_FUNQ_GKIT_FILE]"
                               % gkit_file)
        parser.add_option('--funq-attach-exe',
                          dest='funq_attach_exe',
                          default=env.get('NOSE_FUNQ_ATTACH_EXE')
                          or locate_funq(),
                          help="Complete path to the funq executable."
                               " [NOSE_FUNQ_ATTACH_EXE]")
        parser.add_option('--funq-trace-tests',
                          dest='funq_trace_tests',
                          default=env.get('NOSE_FUNQ_TRACE_TESTS'),
                          help="A file in which start and end of tests will be"
                               " logged. [NOSE_FUNQ_TRACE_TESTS]")
        parser.add_option('--funq-trace-tests-encoding',
                          dest='funq_trace_tests_encoding',
                          default=env.get('NOSE_FUNQ_TRACE_TESTS_ENCODING')
                          or 'utf-8',
                          help="encoding of the file used in"
                               " --funq-trace-tests."
                               " [NOSE_FUNQ_TRACE_TESTS_ENCODING]")
        parser.add_option('--funq-screenshot-folder',
                          dest="funq_screenshot_folder",
                          default=env.get("NOSE_FUNQ_SCREENSHOT_FOLDER")
                          or os.path.realpath("screenshot-errors"),
                          help="Folder to saves screenshots on error."
                               " Default: screenshot-errors."
                               " [NOSE_FUNQ_SCREENSHOT_FOLDER]")
        parser.add_option('--funq-snooze-factor',
                          dest="funq_snooze_factor",
                          default=env.get("NOSE_FUNQ_SNOOZE_FACTOR")
                          or 1.0,
                          help="Allow to specify a factor on every timeout."
                               " Default: 1.0.  [NOSE_FUNQ_SNOOZE_FACTOR]")

    def configure(self, options, cfg):
        Plugin.configure(self, options, cfg)
        if not self.enabled:
            return
        conf_file = options.funq_conf = os.path.realpath(options.funq_conf)
        if not os.path.isfile(conf_file):
            raise Exception(
                "Missing configuration file of funq: `%s`" % conf_file)
        conf = ConfigParser()
        conf.read([conf_file])
        self.app_registry = ApplicationRegistry()
        self.app_registry.register_from_conf(conf, options)
        register_funq_app_registry(self.app_registry)
        self.trace_tests = options.funq_trace_tests
        self.trace_tests_encoding = \
            options.funq_trace_tests_encoding
        self.screenshoter = ScreenShoter(options.funq_screenshot_folder)
        tools.SNOOZE_FACTOR = float(options.funq_snooze_factor)
        FunqPlugin._instance = self

    def beforeTest(self, test):
        message = u"Starting test `%s`" % test.id()
        lines = message_with_sep(message)
        for line in lines:
            LOG.info(line)
        if self.trace_tests:
            with codecs.open(self.trace_tests, 'a',
                             self.trace_tests_encoding) as f:
                f.write('\n'.join(lines))
                f.write('\n')

    def afterTest(self, test):
        message = u"Ending test `%s`" % test.id()
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

    def take_screenshot(self, test):
        if isinstance(test, MultiFunqTestCase):
            if test.__app_config__:
                for k, v in test.__app_config__.iteritems():
                    if v.screenshot_on_error:
                        self.screenshoter.take_screenshot(
                            test.funq[k],
                            '%s [%s]' % (test.id(), k)
                        )
        elif isinstance(test, FunqTestCase):
            if test.__app_config__:
                if test.__app_config__.screenshot_on_error:
                    self.screenshoter.take_screenshot(test.funq, test.id())

    def prepareTestResult(self, result):
        _addError = result.addError
        _addFailure = result.addFailure

        def addError(test, err):
            self.take_screenshot(test.test)
            _addError(test, err)

        def addFailure(test, err):
            self.take_screenshot(test.test)
            _addFailure(test, err)
        result.addError = addError
        result.addFailure = addFailure
