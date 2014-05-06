from nose.tools import assert_equals, assert_true
from funq import screenshoter
from funq.noseplugin import FunqPlugin
import tempfile, shutil, os


class FakeFunqClient(object):
    def __init__(self):
        self.screens = []
        
    def take_screenshot(self, fname, type_):
        self.screens.append(fname)

class ScreenShoterCtx(object):
    def __enter__(self):
        self.base_tmp_dir = tempfile.mkdtemp()
        self.tmp_dir = os.path.join(self.base_tmp_dir, 'images')
        screenshoter.init(self.tmp_dir)
        FunqPlugin._current_test_name = None
        return self
    
    def __exit__(self, type, value, tb):
        shutil.rmtree(self.base_tmp_dir)
        FunqPlugin._current_test_name = None

def test_take_one_screenshot():
    hooq = FakeFunqClient()
    with ScreenShoterCtx() as ctx:
        FunqPlugin._current_test_name = "hello"
        screenshoter.take_screenshot(hooq)
        assert_equals(map(os.path.basename, hooq.screens), ["0.png"])
        assert_true("0.png: hello" in open(os.path.join(ctx.tmp_dir, 'images.txt')).read())

def test_take_screenshots():
    hooq = FakeFunqClient()
    with ScreenShoterCtx() as ctx:
        FunqPlugin._current_test_name = "hello"
        screenshoter.take_screenshot(hooq)
        
        FunqPlugin._current_test_name = "thisisit"
        screenshoter.take_screenshot(hooq)
        
        assert_equals(map(os.path.basename, hooq.screens), ["0.png", "1.png"])
        content = open(os.path.join(ctx.tmp_dir, 'images.txt')).read()
        assert_true("0.png: hello" in content)
        assert_true("1.png: thisisit" in content)
