from nose.tools import assert_equals, assert_true
from funq import screenshoter
from funq.noseplugin import FunqPlugin
import tempfile, shutil, os


class FakeFunqClient(object):
    def __init__(self):
        self.screens = []
        
    def take_screenshot(self, fname, type_):
        self.screens.append(fname)

class ScreenShoterCtx(screenshoter.ScreenShoter):
    def __init__(self):
        screenshoter.ScreenShoter.__init__(self, tempfile.mkdtemp())
    
    def __enter__(self):
        return self
    
    def __exit__(self, type, value, tb):
        shutil.rmtree(self.working_folder)

def test_take_one_screenshot():
    funq = FakeFunqClient()
    with ScreenShoterCtx() as ctx:
        ctx.take_screenshot(funq, "hello")
        assert_equals(map(os.path.basename, funq.screens), ["0.png"])
        assert_true("0.png: hello" in open(os.path.join(ctx.working_folder, 'images.txt')).read())

def test_take_screenshots():
    funq = FakeFunqClient()
    with ScreenShoterCtx() as ctx:
        ctx.take_screenshot(funq, "hello")
        
        ctx.take_screenshot(funq, "thisisit")
        
        assert_equals(map(os.path.basename, funq.screens), ["0.png", "1.png"])
        content = open(os.path.join(ctx.working_folder, 'images.txt')).read()
        assert_true("0.png: hello" in content)
        assert_true("1.png: thisisit" in content)
