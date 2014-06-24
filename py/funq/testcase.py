# -*- coding: utf-8 -*-

"""
Module d'intégration de funq comme TestCase.
"""

import unittest
import weakref
from funq.client import ApplicationContext
from funq import screenshoter

def parameterized(func_suffix, *args, **kwargs):
    """
    Décorateur de fonction paramétrée.
    """
    def wrapped(func):
        if not hasattr(func, 'parameters'):
            func.parameters = []
        func.parameters.append((func_suffix, args, kwargs))
        return func
    return wrapped

def with_parameters(parameters):
    def wrapped(func):
        func.parameters = parameters
        return func
    return wrapped

def wraps_parameterized(func, func_suffix, args, kwargs):
    def wrapper(self):
        return func(self, *args, **kwargs)
    wrapper.__name__ = func.__name__ + '_' + func_suffix
    wrapper.__doc__ = '[%s] %s' % (func_suffix, func.__doc__)
    return wrapper

class MetaParameterized(type):
    def __new__(cls, name, bases, attrs):
        for k, v in attrs.items():
            if callable(v) and hasattr(v, 'parameters'):
                for func_suffix, args, kwargs in v.parameters:
                    wrapper = wraps_parameterized(v,func_suffix, args, kwargs)
                    if wrapper.__name__ in attrs:
                        raise KeyError("%s is already a defined method on %s" %
                                        (wrapper.__name__, name))
                    attrs[wrapper.__name__] = wrapper
                del attrs[k]
                    
        return type.__new__(cls, name, bases, attrs)

class FunqTestCase(unittest.TestCase):
    """
    Un TestCase basé sur funq.
    
    Le temps de l'exécution de chaque méthode de test, l'ApplicationContext
    est créé. L'instance de :class:`funq.client.FunqClient` est accessible par
    la variable membre self.funq.
    
    """
    __metaclass__ = MetaParameterized
    screenshot_on_error = False
    CFG = None
    
    def _create_application_context(self):
        cfg = self.CFG
        if cfg is None:
            raise RuntimeError(u"%s.CFG must be set" % self.__class__.__name__)
        return ApplicationContext(self.CFG)
    
    def setUp(self):
        self.__ctx = self._create_application_context()
        self.funq = weakref.proxy(self.__ctx.funq)
        self.addCleanup(self.__delete_context)
    
    def run(self, result=None):
        try:
            unittest.TestCase.run(self, result)
        except (SystemExit, KeyboardInterrupt, unittest.SkipTest):
            raise
        except:
            if self.screenshot_on_error:
                screenshoter.take_screenshot(self.funq)
            raise
    
    def __delete_context(self):
        del self.__ctx
