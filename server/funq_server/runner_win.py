from funq_server.runner import RunnerInjector
import winappdbg
import time

class WindowsRunnerInjector(RunnerInjector):
    max_wait = 20 # 20 seconds to let the process starts well

    def start_subprocess(self):
        RunnerInjector.start_subprocess(self)
        # wait for the process to be be running...
        proc = winappdbg.Process(self._proc.pid)
        start = time.time()
        while 1:
            # wait for QT to be loaded
            if self._proc.poll() is not None:
                raise RuntimeError("The process has finished with an error code"
                                   " of %d before we could hook it with funq."
                                   % self._proc.returncode)
        
            if start + self.max_wait < time.time():
                self._proc.terminate()
                raise RuntimeError("Error while waiting for subprocess to be launched."
                                   " Is the executable linked to qt4 ?")
            proc.scan_modules()
            lib_names = [lib.get_name() for lib in proc.iter_modules()]
            if 'qtguid4' in lib_names or 'qtgui4' in lib_names:
                break
            time.sleep(0.01)
        
        time.sleep(1) # wait a bit, and hope that QT is now really initialized !
        # we can inject the dll
        proc.inject_dll(self.library_path)
