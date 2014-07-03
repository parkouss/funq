from setuptools import setup, Command
from setuptools.command.install import install as _install
from distutils.command.build import build as _build
import subprocess
import shutil
import os
import platform

IS_WINDOWS = platform.system() == 'Windows'

install_requires = []
if IS_WINDOWS:
    install_requires.append('winappdbg')

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
        self.qmake_path = None
        self.make_path = None
        
        if IS_WINDOWS:
            self.funqlib_name = 'Funq.dll'
        else:
            self.funqlib_name = 'libFunq.so'

    def finalize_options(self):
        self.set_undefined_options('build',
                                   ('force', 'force'),
                                   ('debug', 'debug'),
                                   ('build_lib', 'build_lib'))
        if self.qmake_path is None:
            self.qmake_path = 'qmake' if IS_WINDOWS else 'qmake-qt4'
        if self.make_path is None:
            self.make_path = 'make'
    
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
        print 'running %s' % qmake_cmd
        subprocess.check_call(qmake_cmd)
        make_cmd = [self.make_path]
        if IS_WINDOWS:
            make_cmd += ['debug' if self.debug else 'release']
        print 'running %s' % make_cmd
        subprocess.check_call(make_cmd, shell=True)
        
        shutil.copy2(os.path.join('bin', self.funqlib_name), self.funqlib_out_path())

    def get_outputs(self):
        return [self.funqlib_out_path()]

class build(_build):
    sub_commands = _build.sub_commands + [('build_libfunq', None)]

class install(_install):
    sub_commands = _install.sub_commands + [('build_libfunq', None)]

setup(
    name='funq_server',
    packages=['funq_server'],
    entry_points = {
        'console_scripts': [
            'funq = funq_server.runner:main'
        ]
    },
    cmdclass = {
        'build_libfunq': build_libfunq,
        'build': build,
        'install': install,
    },
    install_requires=install_requires,
)
