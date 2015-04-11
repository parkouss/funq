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

import os
import sys
import subprocess
import platform
import argparse
import funq_server


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
        desc = u"""Start a QT application with a libFunq server injected."""
        parser = argparse.ArgumentParser(description=desc)
        parser.add_argument('-v', '--version', action='version',
                            version=funq_server.__version__)
        parser.add_argument('--pick', action='store_true',
                            help="Use PICK MODE, to find widget's paths")
        parser.add_argument('--port', type=int,
                            help="Specify funq port.")
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
