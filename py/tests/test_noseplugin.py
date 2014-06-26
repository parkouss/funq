from nose.tools import assert_equals, assert_true
from funq import noseplugin, testcase, tools
from optparse import OptionParser
import tempfile

def configured_plugin(argv=[], env={}):
    plugin = noseplugin.FunqPlugin()
    plugin.enabled = True
    parser = OptionParser()
    
    plugin.options(parser)
    options = parser.parse_args(argv)[0]
    plugin.configure(options, None)
    return plugin

class FakeTest(object):
    def __init__(self, test_id):
        self.test_id = test_id
    
    def id(self):
        return self.test_id

def test_simple_configure():
    tools.SNOOZE_FACTOR = 1.0
    
    funqconf = tempfile.NamedTemporaryFile()
    plugin = configured_plugin(["--funq-conf", funqconf.name])
    
    assert_equals(1.0, tools.SNOOZE_FACTOR)
    assert_true(plugin.screenshoter is not None)
    assert_equals(plugin.trace_tests, None)

def test_snooze_factor_configure():
    tools.SNOOZE_FACTOR = 1.0
    
    funqconf = tempfile.NamedTemporaryFile()
    plugin = configured_plugin(["--funq-conf", funqconf.name, '--funq-snooze-factor', '3.2'])
    
    assert_equals(3.2, tools.SNOOZE_FACTOR)

def test_before_after():
    noseplugin.FunqPlugin._current_test_name = None
    
    funqconf = tempfile.NamedTemporaryFile()
    plugin = configured_plugin(["--funq-conf", funqconf.name])
    
    test = FakeTest("id of test")
    
    plugin.beforeTest(test)
    assert_equals(noseplugin.FunqPlugin.current_test_name(), "id of test")
    
    plugin.afterTest(test)
    assert_equals(noseplugin.FunqPlugin.current_test_name(), None)
