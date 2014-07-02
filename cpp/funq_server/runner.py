# -*- coding: utf-8 -*-
import os
import sys
import subprocess
import platform
import argparse

class RunnerInjector(object):
    def __init__(self, library_path, args, env):
        self.library_path = library_path
        self.args = args
        self.env = env
        self._proc = None

    def run(self):
        self.start_subprocess()
        return self.wait__subprocess()

    def start_subprocess(self):
        self._proc = subprocess.Popen(self.args, env=self.env)

    def wait__subprocess(self):
        return self._proc.wait()

class Runner(object):
    def __init__(self):
        self.system = platform.system()
    
    def _parse_args(self, argv=None):
        parser = argparse.ArgumentParser()
        parser.add_argument('--pick', action='store_true',
                            help="Passe funq en mode pick, pour localiser"
                                 " les widgets.")
        parser.add_argument('--port', type=int,
                            help="Specifie le port d'ecoute de funq.")
        parser.add_argument('command', nargs=argparse.REMAINDER)
        return parser.parse_args(argv)
    
    def _find_library(self):
        this_dir = os.path.dirname(os.path.realpath(__file__))
        if self.system == 'Windows':
            library_name = 'Funq.dll'
        else:
            library_name = 'libFunq.so'
        return os.path.join(this_dir, library_name)
    
    def _create_injector_class(self):
        if self.system == 'Windows':
            from funq_server.runner_win import WindowsRunnerInjector as RI
        else:
            from funq_server.runner_linux import LinuxRunnerInjector as RI
        return RI
    
    def run(self, argv=None):
        opts = self._parse_args(argv)
        env = dict(os.environ)
        
        if opts.pick:
            env['FUNQ_MODE_PICK'] = '1'
        if opts.port is not None:
            env['FUNQ_PORT'] = str(opts.port)
    
        library_path = self._find_library()
        if not os.path.isfile(library_path):
            raise RuntimeError("Unable to find funq library %r" % library_path)
        
        injector_class = self._create_injector_class()
        injector = injector_class(library_path, opts.command, env)
        return injector.run()

def main():
    runner = Runner()
    sys.exit(runner.run())

if __name__ == '__main__':
    main()
