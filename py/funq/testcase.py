# -*- coding: utf-8 -*-

"""
Module d'intégration de funq comme TestCase.
"""

import unittest
import weakref
from functools import wraps
from funq.client import ApplicationContext
import os
import re
import inspect

class AssertionSuccessError(AssertionError):
    """
    Exception qui sera levée si une méthode décoré par @todo ne
    lève aucune exception
    """

    def __init__(self, name):
        super(AssertionSuccessError, self).__init__()
        self.name = name

    def __str__(self):
        return u"Le test %s s'est bien déroulé alors qu'il est marqué " \
               u"en TODO" % self.name

    def __rep__(self):
        return self.__str__()


def todo(skip_message, exception_cls=AssertionError):
    """
    Décorateur qui "skip" un test si ce dernier échoue.
    :param skip_message: Message d'erreur affiché lorsque le test est passé
    """
    def wrapped(func):
        """
        Fonction surchargée
        """
        @wraps(func)
        def wrapper(*args, **kwargs):
            """
            Fonction à surcharger
            """
            try:
                func(*args, **kwargs)
            except exception_cls as err:
                err = u"%s" % err
                if isinstance(err, unicode):
                    err = err.encode('utf-8', errors='ignore')  # pylint: disable=E1103
                skip_msg = skip_message.encode('utf-8', errors='ignore')
                raise unittest.SkipTest('\nErreur: %s\n%s' % (err, skip_msg))

            raise AssertionSuccessError(func.__name__)

        return wrapper
    return wrapped


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
    RE_ESCAPE_BAD_CHARS = re.compile(r'[\.\(\) -/]')
    def __new__(cls, name, bases, attrs):
        for k, v in attrs.items():
            if callable(v) and hasattr(v, 'parameters'):
                for func_suffix, args, kwargs in v.parameters:
                    func_suffix = cls.RE_ESCAPE_BAD_CHARS.sub('_', func_suffix)
                    wrapper = wraps_parameterized(v,func_suffix, args, kwargs)
                    if wrapper.__name__ in attrs:
                        raise KeyError("%s is already a defined method on %s" %
                                        (wrapper.__name__, name))
                    attrs[wrapper.__name__] = wrapper
                del attrs[k]
                    
        return type.__new__(cls, name, bases, attrs)


class BaseTestCase(unittest.TestCase):
    __metaclass__ = MetaParameterized
    
    longMessage = True
    
    
    def __init__(self, *args, **kwargs):
        unittest.TestCase.__init__(self, *args, **kwargs)
        self.app_config = None
        self.funq = None
    
    def _create_funq_ctx(self):
        raise NotImplementedError
    
    def setUp(self):
        self.__ctx = self._create_funq_ctx()
        self.addCleanup(self.__delete_funq_ctx)
    
    def __delete_funq_ctx(self):
        del self.__ctx
    
    def _create_funq_app_config(self, app_registry):
        raise NotImplementedError
    
    def run(self, result=None):
        app_registry = result.result.app_registry
        self.app_config = self._create_funq_app_config(app_registry)
        unittest.TestCase.run(self, result)
    
    def id(self):
        cls = self.__class__
        fname = inspect.getsourcefile(cls)[len(os.getcwd()) + 1:]
        return u"%s:%s.%s" % (fname, cls.__name__, self._testMethodName)

class FunqTestCase(BaseTestCase):
    """
    classe de TestCase pour lancer une application et la tester. Hérite de unittest.TestCase.
    
    L'attribut de classe **app_config_name** est requis et doit contenir un nom de
    section dans la configuration valide.

    :var funq: instance de :class:`funq.Client.FunqClient`, permettant de manipuler
               l'application.
    :var app_config: instance de :class:`funq.Client.ApplicationConfig`, permettant
                     de connaitre la configuration de l'application testée
    """
    app_config_name = None
    
    def _create_funq_app_config(self, app_registry):
        return app_registry.config(self.app_config_name)

    def _create_funq_ctx(self):
        ctx = ApplicationContext(self.app_config)
        self.funq = weakref.proxy(ctx.funq)
        return ctx

class MultiFunqTestCase(BaseTestCase):
    """
    classe de TestCase pour lancer plusieurs applications en même temps et les
    tester. Hérite de unittest.TestCase.
    
    L'attribut de classe **app_config_names** est requis et doit contenir une liste
    de noms de section dans la configuration valides.

    :var funq: dictionnaire contenant des :class:`funq.Client.FunqClient`, permettant de manipuler
               les applications. Les clés sont les noms des configurations.
    :var app_config: dictionnaire contenant des :class:`funq.Client.ApplicationConfig`, permettant
                     de connaitre la configuration de l'application testée.
                     Les clés sont les noms des configurations.
    """
    app_config_names = None
    
    def _create_funq_app_config(self):
        return dict([(k, app_registry.config(k)) for k in self.app_config_names])

    def _create_funq_ctx(self):
        ctx = {}
        self.funq = {}
        for k, v in self.app_config.iteritems():
            ctx[k] = ApplicationContext(v)
            self.funq[k] = weakref.proxy(ctx[k].funq)
        return ctx
