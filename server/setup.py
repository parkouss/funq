# -*- coding: utf-8 -*-
from setuptools import setup, Command
from setuptools.command.install import install as _install
from setuptools.command.develop import develop as _develop
from distutils.command.build import build as _build
import subprocess
import shutil
import os
import re
import sys
import platform

IS_WINDOWS = platform.system() == 'Windows'
IS_MAC = platform.system() == 'Darwin'

if sys.version_info < (3, 5):
    sys.exit("Python version must be >= 3.5")


def read(*paths):
    this_dir = os.path.dirname(os.path.realpath(__file__))
    content = open(os.path.join(this_dir, *paths), "rb").read()
    return content.decode("utf-8")


version = re.search("__version__ = '(.+)'", read('funq_server/__init__.py')).group(1)


class build_libfunq(Command):
    """
    Construction de la lib c++.
    """
    user_options = [
        ('build-lib=', 'd', "directory to \"build\" (copy) to"),
        ('force', 'f', "forcibly build everything (ignore file timestamps)"),
        ('cmake-path=', None, "path to the cmake executable"),
        ('make-path=', None, "path to the make executable"),
        ('debug', 'g',
         "compile/link with debugging information"),
        ('inplace', 'i',
         "ignore build-lib and put compiled extensions into the source " +
         "directory alongside your pure Python modules"),
    ]

    boolean_options = ['inplace', 'debug', 'force']

    def initialize_options(self):
        self.build_lib = None
        self.force = None
        self.inplace = None
        self.debug = None
        self.cmake_path = None
        self.make_path = None
        self.qt_version = None

        if IS_WINDOWS:
            self.funqlib_name = 'libFunq.dll'
        elif IS_MAC:
            self.funqlib_name = 'libFunq.dylib'
        else:
            self.funqlib_name = 'libFunq.so'

    def finalize_options(self):
        self.set_undefined_options('build',
                                   ('force', 'force'),
                                   ('debug', 'debug'),
                                   ('build_lib', 'build_lib'))
        if self.cmake_path is None:
            self.cmake_path = os.environ.get('FUNQ_CMAKE_PATH') or 'cmake'
        if self.make_path is None:
            self.make_path = os.environ.get('FUNQ_MAKE_PATH') or 'make'
        if self.qt_version is None:
            self.qt_version = os.environ.get('FUNQ_QT_MAJOR_VERSION')

    def funqlib_out_path(self):
        funqlib_base_dir = '.' if self.inplace else self.build_lib
        return os.path.join(funqlib_base_dir, 'funq_server', self.funqlib_name)

    def run(self):
        if self.force:
            subprocess.call([self.make_path, 'clean'], shell=True)
        buildtype = 'Debug' if self.debug else 'Release'
        cmake_cmd = [
            self.cmake_path, '.',
            '-DCMAKE_BUILD_TYPE={}'.format(buildtype),
        ]
        if self.qt_version is not None:
            cmake_cmd += ['-DQT_MAJOR_VERSION={}'.format(self.qt_version)]
        print('running %s' % cmake_cmd)
        subprocess.check_call(cmake_cmd)

        make_cmd = [self.make_path]
        print('running %s' % make_cmd)
        subprocess.check_call(make_cmd, shell=True)

        lib_path = self.funqlib_out_path()
        lib_dir = os.path.dirname(lib_path)
        if not os.path.isdir(lib_dir):
            os.makedirs(lib_dir)
        shutil.copy2(os.path.join('libFunq', self.funqlib_name), lib_path)

    def get_outputs(self):
        return [self.funqlib_out_path()]


class build(_build):
    sub_commands = _build.sub_commands + [('build_libfunq', None)]


class install(_install):
    def run(self):
        self.run_command('build_libfunq')
        _install.run(self)


class develop(_develop):
    def run(self):
        self.reinitialize_command('build_libfunq', inplace=1)
        self.run_command('build_libfunq')
        _develop.run(self)


setup(
    name='funq-server',
    author="Julien Pagès",
    author_email="j.parkouss@gmail.com",
    url="https://github.com/parkouss/funq",
    description="write FUNctional tests for Qt applications (server)",
    long_description=read("README"),
    version=version,
    packages=['funq_server'],
    entry_points={
        'console_scripts': [
            'funq = funq_server.runner:main'
        ]
    },
    cmdclass={
        'build_libfunq': build_libfunq,
        'build': build,
        'install': install,
        'develop': develop,
    },
    install_requires=[],
    license='CeCILL v1.2',
)
