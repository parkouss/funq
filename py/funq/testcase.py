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
    Exception qui sera levée si une méthode décoré par :func:`todo` passe
    alors que ce n'est pas attendu.
    
    :param name: message d'erreur.
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
    Décorateur qui "skippe" un test si ce dernier échoue, et qui lève une
    exception de type :class:`AssertionSuccessError` si le test passe alors
    que ce n'est pas attendu.
    
    Il est possible de spécifier le type d'erreur pris en compte avec l'argument
    **exception_cls**.
    
    Exemple::
      
      class MyTestCase(FunqTestCase):
          app_config_name = 'ma_conf'
          
          @todo("this test needs something to pass")
          def test_one(self):
              raise AssertionError('this will fail')
    
    :param skip_message: Message d'erreur affiché lorsque le test est passé
    :param exception_cls: Classe d'erreur ou tuple de classes qui indiquent
                          les exceptions à prendre ne compte.
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
    Décorateur de méthode paramétrée. Permet de générer une méthode à partie
    d'une autre méthode et de données.
    
    **func_suffix** est utilisé comme suffixe pour la nouvelle méthode créée
    et doit être unique pour une même méthode. Si **func_suffix** contient
    des caractères spéciaux qui ne peuvent normalement être contenus dans des
    noms de fonction, ils seront remplacés par "_".
    
    Ce décorateur peut être appliqué plusieurs fois sur la même méthode.
    
    Exemple::
      
      # Cet exemple permet de générer deux fonctions:
      #
      # - MyTestCase.test_it_1
      # - MyTestCase.test_it_2
      #
      class MyTestCase(FunqTestCase):
          app_config_name = 'ma_conf'
          
          @parameterized("1", 5, named='nom')
          @parameterized("2", 6, named='nom2')
          def test_it(self, value, named=None):
              print value, named
    
    :param func_suffix: string qui sera utilisé en suffixe de fonction
    :param \*args: les arguments restants seront passés à la méthode décorée
    :param \*\*kwargs: les arguments restants seront passés à la méthode décorée
    """
    def wrapped(func):
        if not hasattr(func, 'parameters'):
            func.parameters = []
        func.parameters.append((func_suffix, args, kwargs))
        return func
    return wrapped


def with_parameters(parameters):
    """
    Décorateur de méthode paramétrée. Fonctione un peu comme :func:`parameterized`
    mais définit les paramètres en un coup.
    
    Exemple::
      
      # Cet exemple permet de générer deux fonctions:
      #
      # - MyTestCase.test_it_1
      # - MyTestCase.test_it_2
      #
      
      DATA = [("1", [5], {'named':'nom'}), ("2", [6], {'named':'nom2'})]
      
      class MyTestCase(FunqTestCase):
          app_config_name = 'ma_conf'
          
          @with_parameters(DATA)
          def test_it(self, value, named=None):
              print value, named
    
    :param parameters: liste de tuple (**func_suffix**, **args**, **kwargs**
                       définissant les paramètres comme dans :func:`todo`.
    """
    def wrapped(func):
        func.parameters = parameters
        return func
    return wrapped


def wraps_parameterized(func, func_suffix, args, kwargs):
    """Internal: pour MetaParameterized"""
    def wrapper(self):
        return func(self, *args, **kwargs)
    wrapper.__name__ = func.__name__ + '_' + func_suffix
    wrapper.__doc__ = '[%s] %s' % (func_suffix, func.__doc__)
    return wrapper


class MetaParameterized(type):
    """
    Metaclasse pour permettre aux classes qui utilisent des méthodes décorées
    avec :func:`parameterized` ou :func:`with_parameters` de générer les nouvelles
    méthodes.
    """
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
    """
    Classe abstraite pour un testcase Funq.
    
    Définit un comportement commun pour le nommage des tests ainsi que par
    l'utilisation de la métaclasse :class:`MetaParameterized` (ce qui permet
    de décorer des méthodes afin de dupliques des méthodes en fonction de
    paramètres).
    
    L'héritage depuis :class:`unittest.TestCase` offre de nombres fonctions
    d'assertions pratiques, comme assertEquals, assertFalse, etc.
    """
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
    classe de TestCase pour lancer une application et la tester.
    
    L'attribut de classe **app_config_name** est requis et doit contenir un nom de
    section dans la configuration valide.

    :var funq: instance de :class:`funq.client.FunqClient`, permettant de manipuler
               l'application.
    :var app_config: instance de :class:`funq.client.ApplicationConfig`, permettant
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
    tester.
    
    L'attribut de classe **app_config_names** est requis et doit contenir une liste
    de noms de section dans la configuration valides.

    :var funq: dictionnaire contenant des :class:`funq.client.FunqClient`, permettant de manipuler
               les applications. Les clés sont les noms des configurations.
    :var app_config: dictionnaire contenant des :class:`funq.client.ApplicationConfig`, permettant
                     de connaitre la configuration de l'application testée.
                     Les clés sont les noms des configurations.
    """
    app_config_names = None
    
    def _create_funq_app_config(self, app_registry):
        return dict([(k, app_registry.config(k)) for k in self.app_config_names])

    def _create_funq_ctx(self):
        ctx = {}
        self.funq = {}
        for k, v in self.app_config.iteritems():
            ctx[k] = ApplicationContext(v)
            self.funq[k] = weakref.proxy(ctx[k].funq)
        return ctx
