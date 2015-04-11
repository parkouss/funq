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

from funq_server.runner import RunnerInjector
import winappdbg
import time


class WindowsRunnerInjector(RunnerInjector):
    max_wait = 20  # 20 seconds to let the process starts well

    def start_subprocess(self):
        RunnerInjector.start_subprocess(self)
        # wait for the process to be be running...
        proc = winappdbg.Process(self._proc.pid)
        start = time.time()
        while 1:
            # wait for QT to be loaded
            if self._proc.poll() is not None:
                raise RuntimeError("The process has finished with an error"
                                   " code of %d before we could hook it with"
                                   " funq." % self._proc.returncode)

            if start + self.max_wait < time.time():
                self._proc.terminate()
                raise RuntimeError("Error while waiting for subprocess to be"
                                   " launched. Is the executable linked to"
                                   " qt4 ?")
            proc.scan_modules()
            lib_names = [lib.get_name() for lib in proc.iter_modules()]
            if 'qtguid4' in lib_names or 'qtgui4' in lib_names:
                break
            time.sleep(0.01)

        # wait a bit, and hope that QT is now really initialized !
        time.sleep(1)
        # we can inject the dll
        proc.inject_dll(self.library_path)
