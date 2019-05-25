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

if sys.version_info < (2, 7):
    sys.exit("Python version must be > 2.7")
elif sys.version_info > (3,) and IS_WINDOWS:
    sys.exit('funq server under windows require winappdbg'
             ' which is not available under python 3 currenly.')

install_requires = []
if IS_WINDOWS:
    install_requires.append('winappdbg')


def read(*paths):
    this_dir = os.path.dirname(os.path.realpath(__file__))
    return open(os.path.join(this_dir, *paths)).read()


version = re.search("__version__ = '(.+)'", read('funq_server/__init__.py')).group(1)


class build_libfunq(Command):
    """
    Construction de la lib c++.
    """
    user_options = [
        ('build-lib=', 'd', "directory to \"build\" (copy) to"),
        ('force', 'f', "forcibly build everything (ignore file timestamps)"),
        ('qmake-path=', None, "path to the qmake executable"),
        ('make-path=', None, "path to the make executable"),
        ('debug', 'g',
         "compile/link with debugging information"),
        ('cxxflags=', None, "custom QMAKE_CXXFLAGS passed to qmake"),
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
        self.cxxflags = None
        self.qmake_path = None
        self.make_path = None

        if IS_WINDOWS:
            self.funqlib_name = 'Funq.dll'
        elif IS_MAC:
            self.funqlib_name = 'libFunq.dylib'
        else:
            self.funqlib_name = 'libFunq.so'

    def finalize_options(self):
        self.set_undefined_options('build',
                                   ('force', 'force'),
                                   ('debug', 'debug'),
                                   ('build_lib', 'build_lib'))
        if self.qmake_path is None:
            self.qmake_path = os.environ.get('FUNQ_QMAKE_PATH') or 'qmake'
        if self.make_path is None:
            self.make_path = os.environ.get('FUNQ_MAKE_PATH') or 'make'
        if self.cxxflags is None:
            self.cxxflags = os.environ.get('FUNQ_CXXFLAGS')

    def funqlib_out_path(self):
        funqlib_base_dir = '.' if self.inplace else self.build_lib
        return os.path.join(funqlib_base_dir, 'funq_server', self.funqlib_name)

    def run(self):
        if self.force:
            subprocess.call([self.make_path, 'clean'], shell=True)
        buildtype = 'Debug' if self.debug else 'Release'
        qmake_cmd = [self.qmake_path, 'CONFIG+=%s' % buildtype, '-r']
        if IS_WINDOWS:
            qmake_cmd += ['-spec', 'win32-g++']
        if self.cxxflags:
            qmake_cmd += ['QMAKE_CXXFLAGS="' + self.cxxflags + '"']
        print('running %s' % qmake_cmd)
        subprocess.check_call(qmake_cmd)
        make_cmd = [self.make_path]
        if IS_WINDOWS:
            make_cmd += ['debug' if self.debug else 'release']
        print('running %s' % make_cmd)
        subprocess.check_call(make_cmd, shell=True)

        lib_path = self.funqlib_out_path()
        lib_dir = os.path.dirname(lib_path)
        if not os.path.isdir(lib_dir):
            os.makedirs(lib_dir)
        shutil.copy2(os.path.join('bin', self.funqlib_name), lib_path)

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
    install_requires=install_requires,
    license='CeCILL v1.2',
)
