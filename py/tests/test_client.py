from nose.tools import *
from funq import client
import os
import weakref

from ConfigParser import ConfigParser, NoOptionError

class ApplicationConfig(client.ApplicationConfig):
    def create_context(self):
        class O: pass
        class Ctx:
            funq = O()
        ctx = Ctx()
        
        return ctx

class GlobalOptions(object):
    def __init__(self, **kwds):
        for k, v in kwds.iteritems():
            setattr(self, k, v)

def test_ApplicationConfigWithHooqContextDeleted():
    """
    Teste que le contexte soit bien detruit avec with_hooq
    """
    appconfig = ApplicationConfig("toto")
    
    ref = []
    @appconfig.with_hooq
    def func(obj):
        ref.append(weakref.ref(obj))
    
    func()
    
    assert_equals(len(ref), 1)
    assert_equals(ref[0](), None) # objet detruit

def test_MultiApplicationConfigWithHooqContextDeleted():
    """
    Teste que le contexte soit bien detruit avec with_hooq
    """
    appconfig = client.MultiApplicationConfig([
        ApplicationConfig("toto"),
        ApplicationConfig("titi")])
    
    ref = []
    @appconfig.with_hooq
    def func(obj1, obj2):
        ref.append(weakref.ref(obj1))
        ref.append(weakref.ref(obj2))
    
    func()
    
    assert_equals(len(ref), 2)
    assert_equals(ref[0](), None) # objet detruit
    assert_equals(ref[1](), None) # objet detruit

class TestApplicationConfigFromConf:
    
    def setup(self):
        self.conf = ConfigParser()
        self.conf.add_section('my')
    
    def set_opt(self, name, value):
        self.conf.set('my', name, value)
    
    def createApplicationConfig(self):
        return ApplicationConfig.from_conf(self.conf, 'my',
                GlobalOptions(funq_conf=os.path.join(os.getcwd(), 'my.conf')))
    
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

    def test_multi_config(self):
        c1, c2 =  ApplicationConfig('useless'),  ApplicationConfig('useless')
        self.reg.register_config('c1', c1)
        self.reg.register_config('c2', c2)
        
        multi = self.reg.multi_config(('c2', 'c1'))
        
        assert_is_instance(multi, client.MultiApplicationConfig)
        assert_equals(multi, (c2, c1))
