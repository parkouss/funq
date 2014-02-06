from nose.tools import *
from scletest import tools

def test_wait_for():
    def func():
        return True
    assert_true(tools.wait_for(func, 0.0))

@raises(tools.TimeOutError)
def test_wait_for_timeout():
    def func():
        return False
    tools.wait_for(func, 0.0)

@raises(Exception)
def test_wait_for_custom_exc():
    def func():
        return Exception()
    tools.wait_for(func, 0.0)
