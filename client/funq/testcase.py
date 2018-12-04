# -*- coding: utf-8 -*-

# Copyright: SCLE SFE
# Contributor: Julien Pagès <j.parkouss@gmail.com>
#
# This software is a computer program whose purpose is to test graphical
# applications written with the QT framework (http://qt.digia.com/).
#
# This software is governed by the CeCILL v2.1 license under French law and
# abiding by the rules of distribution of free software.  You can  use,
# modify and/ or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL v2.1 license and that you accept its terms.

"""
funq integration in TestCase subclasses.
"""

import unittest
import weakref
from functools import wraps
from funq.client import ApplicationContext
import os
import re
import inspect


# python 3 compatibility
# https://stackoverflow.com/questions/11301138/how-to-check-if-variable-is-string-with-python-2-and-3-compatibility)
try:
    unicode
except NameError:
    unicode = str


class AssertionSuccessError(AssertionError):

    """
    Exception which will be raised if method decorated with :func:`todo`
    pass (it is not expected).

    :param name: error message.
    """

    def __init__(self, name):
        super(AssertionSuccessError, self).__init__()
        self.name = name

    def __str__(self):
        return u"Test %s passed but it is decorated as TODO" % self.name

    def __rep__(self):
        return self.__str__()


def todo(skip_message, exception_cls=AssertionError):
    """
    A decorator to skip a test on given exception types. If the decorated
    test pass, an exception :class:`AssertionSuccessError` will be thrown.

    It is possible to specify which type of Exception is handled with the
    **exception_cls** argument.

    Example::

      class MyTestCase(FunqTestCase):
          __app_config_name__ = 'ma_conf'

          @todo("this test needs something to pass")
          def test_one(self):
              raise AssertionError('this will fail')

    :param skip_message: error message when test is skipped
    :param exception_cls: Exception type or tuple of Exception type that are
                          handled to skip a test.
    """
    def wrapped(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            try:
                func(*args, **kwargs)
            except exception_cls as err:
                err = u"%s" % err
                if isinstance(err, unicode):
                    err = err.encode(
                        'utf-8', errors='ignore')  # pylint: disable=E1103
                skip_msg = skip_message.encode('utf-8', errors='ignore')
                raise unittest.SkipTest('\nError: %s\n%s' % (err, skip_msg))

            raise AssertionSuccessError(func.__name__)

        return wrapper
    return wrapped


def parameterized(func_suffix, *args, **kwargs):
    """
    A decorator that can generate methods given a base method and some data.

    **func_suffix** is used as a suffix for the new created method and must be
    unique given a base method. if **func_suffix** countains characters that
    are not allowed in normal python function name, these characters will be
    replaced with "_".

    This decorator can be used more than once on a single base method. The
    class must have a metaclass of :class:`MetaParameterized`.

    Example::

      # This example will generate two methods:
      #
      # - MyTestCase.test_it_1
      # - MyTestCase.test_it_2
      #
      class MyTestCase(FunqTestCase):
          __app_config_name__ = 'ma_conf'

          @parameterized("1", 5, named='nom')
          @parameterized("2", 6, named='nom2')
          def test_it(self, value, named=None):
              print value, named

    :param func_suffix: will be used as a suffix for the new method
    :param `*args`: arguments to pass to the new method
    :param `**kwargs`: named arguments to pass to the new method
    """
    def wrapped(func):
        if not hasattr(func, 'parameters'):
            func.parameters = []
        func.parameters.append((func_suffix, args, kwargs))
        return func
    return wrapped


def with_parameters(parameters):
    """
    A decorator that can generate methods given a base method and some data.
    Acts like :func:`parameterized`, but define all methods in one call.

    Example::

      # This example will generate two methods:
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

    :param parameters: list of tuples (**func_suffix**, **args**, **kwargs**)
                       defining parameters like in :func:`todo`.
    """
    def wrapped(func):
        func.parameters = parameters
        return func
    return wrapped


def wraps_parameterized(func, func_suffix, args, kwargs):
    """Internal: for MetaParameterized"""

    def wrapper(self):
        return func(self, *args, **kwargs)
    wrapper.__name__ = func.__name__ + '_' + func_suffix
    wrapper.__doc__ = '[%s] %s' % (func_suffix, func.__doc__)
    return wrapper


class MetaParameterized(type):

    """
    A metaclass that allow a class to use decorators like :func:`parameterized`
    or :func:`with_parameters` to generate new methods.
    """
    RE_ESCAPE_BAD_CHARS = re.compile(r'[\.\(\) -/]')

    def __new__(cls, name, bases, attrs):
        for k, v in attrs.items():
            if callable(v) and hasattr(v, 'parameters'):
                for func_suffix, args, kwargs in v.parameters:
                    func_suffix = cls.RE_ESCAPE_BAD_CHARS.sub('_', func_suffix)
                    wrapper = wraps_parameterized(v, func_suffix, args, kwargs)
                    if wrapper.__name__ in attrs:
                        raise KeyError("%s is already a defined method on %s" %
                                       (wrapper.__name__, name))
                    attrs[wrapper.__name__] = wrapper
                del attrs[k]

        return type.__new__(cls, name, bases, attrs)


class declared_attr(property):

    """
    Allow to write a class method that will be accessible as a class
    attribute.
    """

    def __init__(self, fget, *arg, **kw):
        super(declared_attr, self).__init__(fget, *arg, **kw)
        self.__doc__ = fget.__doc__

    def __get__(desc, self, cls):  # pylint: disable=E0213
        return desc.fget(cls)


def funq_app_config(confname):
    """
    Returns the config named *confname*.
    """
    return BaseTestCase.__app_registry__.config(confname)


def register_funq_app_registry(registry):
    """
    Saves the configurations registry. Must be called before uses of tests
    classes.
    """
    BaseTestCase.__app_registry__ = registry


class BaseTestCase(unittest.TestCase):

    """
    Abstract class of a testcase for Funq.

    It defines a common behaviour to name tests methods and uses the metaclass
    :class:`MetaParameterized` that allows to generate methods from data.

    It inherits from :class:`unittest.TestCase`, thus allowing to use very
    useful methods like assertEquals, assertFalse, etc.
    """
    __metaclass__ = MetaParameterized
    __app_registry__ = None

    longMessage = True

    def __init__(self, *args, **kwargs):
        unittest.TestCase.__init__(self, *args, **kwargs)
        self.funq = None

    def _create_funq_ctx(self):
        raise NotImplementedError

    def setUp(self):
        self.__ctx = self._create_funq_ctx()
        self.addCleanup(self.__delete_funq_ctx)

    def __delete_funq_ctx(self):
        del self.__ctx

    def id(self):
        cls = self.__class__
        fname = inspect.getsourcefile(cls)[len(os.getcwd()) + 1:]
        return u"%s:%s.%s" % (fname, cls.__name__, self._testMethodName)


class FunqTestCase(BaseTestCase):

    """
    A testcase to launch an application and write tests against it.

    The class attribute **__app_config_name__** is required and must contains
    the name of a section in the funq configuration file. A class attribute
    **__app_config__** will then be automatically created to give access to
    the configuration of the application
    (:class:`funq.client.ApplicationConfig`).

    :var funq: instance of :class:`funq.client.FunqClient`, allowing to
               manipulate the application.
    """
    __app_config_name__ = None

    @declared_attr
    def __app_config__(cls):  # pylint: disable=E0213
        if cls.__app_config_name__ is not None:
            return cls.__app_registry__.config(cls.__app_config_name__)

    def _create_funq_ctx(self):
        ctx = ApplicationContext(self.__app_config__)
        self.funq = weakref.proxy(ctx.funq)
        return ctx


class MultiFunqTestCase(BaseTestCase):

    """
    A testcase to launch multiple applications at the same time and write tests
    against them.

    The class attribute **__app_config_names__** is required and must contains
    a list of section's names in the funq configuration file. A class attribute
    **__app_configs__** will then be automatically created to give access to
    the configurations of the application (a dict with values of type
    :class:`funq.client.ApplicationConfig`, where the keys are configuration
    names).

    :var funq: a dict that contains :class:`funq.client.FunqClient`, allowing
               to manipulate the application. Keys are configuration names.
    """
    __app_config_names__ = None

    @declared_attr
    def __app_config__(cls):  # pylint: disable=E0213
        if cls.__app_config_names__ is not None:
            return dict([(k, cls.__app_registry__.config(k))
                         for k in cls.__app_config_names__])

    def _create_funq_ctx(self):
        ctx = {}
        self.funq = {}
        for k, v in self.__app_config__.iteritems():
            ctx[k] = ApplicationContext(v)
            self.funq[k] = weakref.proxy(ctx[k].funq)
        return ctx
