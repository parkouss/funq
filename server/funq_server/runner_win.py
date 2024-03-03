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
from ctypes import windll, wintypes, byref
import time

# Useful resources regarding DLL injection:
#
# - https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices  # noqa: E501
# - https://blog.nettitude.com/uk/dll-injection-part-two
# - https://stackoverflow.com/questions/27332509/createremotethread-on-loadlibrary-and-get-the-hmodule-back  # noqa: E501
# - https://github.com/numaru/injector

# Constants from Windows API documentation.
PROCESS_CREATE_THREAD = 0x0002
PROCESS_VM_OPERATION = 0x0008
PROCESS_VM_WRITE = 0x0020
MEM_COMMIT = 0x00001000
MEM_RESERVE = 0x00002000
PAGE_READWRITE = 0x04


class WindowsRunnerInjector(RunnerInjector):
    def start_subprocess(self):
        # Start the process.
        RunnerInjector.start_subprocess(self)

        # Unfortunately we do not know when the process startup is really
        # complete and Qt libraries are loaded. When starting the injection
        # too early, it does not work 100% reliable (in rare cases, the
        # process freezes or crashes). When slightly delaying the injection,
        # it seems to work more reliable. One seconds seems to be a safe
        # choice to also make it reliable if the system is very busy.        #
        # Hopefully someone finds a better way some day (without delay)...
        time.sleep(1.0)

        # Check if the process is still running.
        if self._proc.poll() is not None:
            raise RuntimeError("The process has finished with an error"
                               " code of %d before we could hook it with"
                               " funq." % self._proc.returncode)

        # Inject the DLL now.
        try:
            self._inject_dll(self._proc.pid, self.library_path)
        except Exception:
            try:
                # Injection failed, try to terminate the process.
                self._proc.terminate()
            except Exception as e:
                print("Failed to terminate process: {}".format(e))
            raise

    def _inject_dll(self, pid, dll_path):
        # Get handle to kernel32.dll.
        kernel32_handle = windll.kernel32.GetModuleHandleA(b"kernel32.dll")
        if not kernel32_handle:
            self._raise_windows_error("GetModuleHandleA()", kernel32_handle)

        # Get handle to LoadLibraryA().
        loadlibrary_address = windll.kernel32.GetProcAddress(
            kernel32_handle, b"LoadLibraryA")
        if not loadlibrary_address:
            self._raise_windows_error("GetProcAddress()", loadlibrary_address)

        # Get handle to the running process.
        process_handle = windll.kernel32.OpenProcess(
            PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE,
            0, pid)
        if not process_handle:
            self._raise_windows_error("OpenProcess()", process_handle)

        # Allocate memory for the DLL path.
        dll_path = dll_path.encode("ascii")
        path_address = windll.kernel32.VirtualAllocEx(
            process_handle, 0, len(dll_path), MEM_COMMIT | MEM_RESERVE,
            PAGE_READWRITE)
        if not path_address:
            self._raise_windows_error("VirtualAllocEx()", path_address)

        # Write DLL path into the allocated memory region.
        success = windll.kernel32.WriteProcessMemory(
            process_handle, path_address, dll_path, len(dll_path), None)
        if not success:
            self._raise_windows_error("WriteProcessMemory()", success)

        # Create and start new thread in the process. The entry point of the
        # new thread is LoadLibraryA() with our DLL path as argument.
        thread_handle = windll.kernel32.CreateRemoteThread(
            process_handle, 0, 0, loadlibrary_address, path_address, 0, None)
        if not thread_handle:
            self._raise_windows_error("CreateRemoteThread()", thread_handle)

        # Release process handle since we no longer need it.
        success = windll.kernel32.CloseHandle(process_handle)
        if not success:
            self._raise_windows_error("CloseHandle()", success)

        # Wait (with 10s timeout) until the thread exited, i.e. our DLL
        # injection either succeeded or failed.
        error = windll.kernel32.WaitForSingleObject(thread_handle, 10000)
        if error:
            self._raise_windows_error("WaitForSingleObject()", error)

        # Get the exit code of our thread, which corresponds to the return
        # value of LoadLibraryA() so we can check if the DLL was loaded
        # successfully or not.
        libfunq_handle = wintypes.DWORD(0)
        success = windll.kernel32.GetExitCodeThread(
            thread_handle, byref(libfunq_handle))
        if not success:
            self._raise_windows_error("GetExitCodeThread()", success)
        if not libfunq_handle:
            self._raise_windows_error("LoadLibraryA()", libfunq_handle)

        # Release thread handle since we no longer need it.
        success = windll.kernel32.CloseHandle(thread_handle)
        if not success:
            self._raise_windows_error("CloseHandle()", success)

    def _raise_windows_error(self, function_name, return_value):
        """
        Helper function to raise an error returned by a WIN32 API function.
        """
        last_error = windll.kernel32.GetLastError()
        message = "Failed to inject DLL! "
        message += "{} returned 0x{:X}. ".format(function_name, return_value)
        message += "The last error is 0x{:X}. ".format(last_error)
        message += "Maybe x86/x64 mismatch between python.exe and Qt DLLs?"
        raise RuntimeError(message)
