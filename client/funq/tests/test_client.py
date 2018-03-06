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

from nose.tools import assert_equals, raises
from funq import client
import os
import subprocess

from ConfigParser import ConfigParser, NoOptionError


class ApplicationConfig(client.ApplicationConfig):
    pass


class GlobalOptions(object):

    def __init__(self, **kwds):
        for k, v in kwds.iteritems():
            setattr(self, k, v)


class TestApplicationConfigFromConf:

    def setup(self):
        self.conf = ConfigParser()
        self.conf.add_section('my')

    def set_opt(self, name, value):
        self.conf.set('my', name, value)

    def createApplicationConfig(self):
        return ApplicationConfig.from_conf(
            self.conf, 'my',
            GlobalOptions(funq_conf=os.path.join(os.getcwd(), 'my.conf'))
        )

    @raises(NoOptionError)
    def test_require_executable(self):
        self.createApplicationConfig()

    def test_abs_executable(self):
        self.set_opt('executable', os.getcwd())
        appconf = self.createApplicationConfig()
        assert_equals(appconf.executable, os.getcwd())

    def test_nonabs_executable(self):
        self.set_opt('executable', 'toto')
        appconf = self.createApplicationConfig()
        assert_equals(appconf.executable, os.path.join(os.getcwd(), 'toto'))

    def test_args(self):
        self.set_opt('executable', 'toto')
        self.set_opt('args', 'toto "titi 1" 2')
        appconf = self.createApplicationConfig()
        assert_equals(appconf.args, ['toto', 'titi 1', '2'])

    def test_port(self):
        self.set_opt('executable', 'toto')
        self.set_opt('funq_port', '12000')
        appconf = self.createApplicationConfig()
        assert_equals(appconf.funq_port, 12000)

    def test_timeout_connection(self):
        self.set_opt('executable', 'toto')
        self.set_opt('timeout_connection', '5')
        appconf = self.createApplicationConfig()
        assert_equals(appconf.timeout_connection, 5)

    def test_abs_aliases(self):
        self.set_opt('executable', 'toto')
        self.set_opt('aliases', os.getcwd())
        appconf = self.createApplicationConfig()
        assert_equals(appconf.aliases, os.getcwd())

    def test_nonabs_aliases(self):
        self.set_opt('executable', 'toto')
        self.set_opt('aliases', 'titi')
        appconf = self.createApplicationConfig()
        assert_equals(appconf.aliases, os.path.join(os.getcwd(), 'titi'))

    def test_stdout_null(self):
        self.set_opt('executable', 'toto')
        self.set_opt('executable_stdout', 'NULL')
        appconf = self.createApplicationConfig()
        assert_equals(appconf.executable_stdout, os.devnull)


class TestApplicationRegistry:

    def setup(self):
        self.reg = client.ApplicationRegistry()

    def test_register_from_conf(self):
        conf = ConfigParser()
        conf.add_section('example')
        exe = os.path.join(os.getcwd(), 'titi')
        conf.set('example', 'executable', exe)

        self.reg.register_from_conf(conf, GlobalOptions(funq_conf='.'))

        assert_equals(len(self.reg.confs), 1)

        assert_equals(self.reg.config('example').executable, exe)


class FakePopen(object):

    @classmethod
    def patch_subprocess_popen(cls, func):
        def patched_subprocess_ctx(*a, **kwa):
            oldpopen = subprocess.Popen
            subprocess.Popen = FakePopen
            try:
                func(*a, **kwa)
            finally:
                subprocess.Popen = oldpopen

        patched_subprocess_ctx.__name__ = func.__name__
        patched_subprocess_ctx.__doc__ = func.__doc__
        return patched_subprocess_ctx

    def __init__(self, command, **kwargs):
        self.command = command
        self.kwargs = kwargs
        self.pid = -1
        self.returncode = 0

    def poll(self):
        return self.returncode


class TestApplicationContext:

    @FakePopen.patch_subprocess_popen
    def test_start(self):
        class O:
            funq_attach_exe = 'funq'
        appconf = client.ApplicationConfig(
            executable='command',
            global_options=O(),
        )

        ctx = client.ApplicationContext(
            appconf, client_class=lambda *a, **kwa: None)
        assert_equals(ctx._process.command, ['funq', 'command'])

    @FakePopen.patch_subprocess_popen
    def test_start_with_valgrind(self):
        class O:
            funq_attach_exe = 'funq'
        appconf = client.ApplicationConfig(
            executable='command',
            with_valgrind=True,
            valgrind_args=[],
            global_options=O(),
        )

        ctx = client.ApplicationContext(
            appconf, client_class=lambda *a, **kwa: None)
        assert_equals(ctx._process.command, ['funq', 'valgrind', 'command'])
