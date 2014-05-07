from nose.tools import *
from funq import tools
import time, sys, os

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

def test_wait_for_some_time():
    t = time.time()
    def func():
        return t + 0.05 < time.time()
    assert_true(tools.wait_for(func, 0.1))

def test_which():
    assert_equals(sys.executable, tools.which(sys.executable))

def test_which_with_pass():
    path, fname = os.path.split(sys.executable)
    old_env = os.environ
    env = dict(PATH=path)
    os.environ = env
    try:
        assert_equals(sys.executable, tools.which(fname))
    finally:
        os.environ = old_env

def test_apply_snooze_factor():
    tools.SNOOZE_FACTOR = 3.2
    assert_equals(6.4, tools.apply_snooze_factor(2))
    tools.SNOOZE_FACTOR = 1.0

def test_wait_for_some_time_with_snooze_factor():
    tools.SNOOZE_FACTOR = 4.0
    t = time.time()
    def func():
        return t + 0.05 < time.time()
    assert_true(tools.wait_for(func, 0.025))
    tools.SNOOZE_FACTOR = 1.0
