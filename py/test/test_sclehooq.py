from nose.tools import *
from scletest import sclehooq
import os

from ConfigParser import ConfigParser, NoOptionError

class ApplicationConfig(sclehooq.ApplicationConfig):
    def create_context(self):
        return self

class TestApplicationConfigFromConf:
    
    def setup(self):
        self.conf = ConfigParser()
        self.conf.add_section('my')
    
    def set_opt(self, name, value):
        self.conf.set('my', name, value)
    
    def createApplicationConfig(self):
        return ApplicationConfig.from_conf(self.conf, 'my', os.getcwd())
    
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
        self.set_opt('hooq_port', '12000')
        appconf = self.createApplicationConfig()
        assert_equals(appconf.hooq_port, 12000)

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
