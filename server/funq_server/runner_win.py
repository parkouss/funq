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
from ctypes import wintypes, byref
import ctypes
import time

# Useful resources regarding DLL injection:
#
# - https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices  # noqa: E501
# - https://blog.nettitude.com/uk/dll-injection-part-two
# - https://stackoverflow.com/questions/17392721/error-invalid-parameter-error-57-when-calling-createremotethread-with-python-3-2/17524073#17524073  # noqa: E501
# - https://stackoverflow.com/questions/27332509/createremotethread-on-loadlibrary-and-get-the-hmodule-back  # noqa: E501
# - https://github.com/numaru/injector

# Constants from Windows API documentation.
PROCESS_CREATE_THREAD = 0x0002
PROCESS_VM_OPERATION = 0x0008
PROCESS_VM_WRITE = 0x0020
MEM_COMMIT = 0x1000
MEM_RESERVE = 0x2000
MEM_RELEASE = 0x8000
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
        # choice to also make it reliable if the system is very busy.
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
        # Get handle to kernel32.dll and prepare functions.
        kernel32 = ctypes.WinDLL('kernel32.dll', use_last_error=True)
        kernel32.OpenProcess.restype = wintypes.HANDLE
        kernel32.OpenProcess.argtypes = (
            wintypes.DWORD,  # dwDesiredAccess
            wintypes.BOOL,   # bInheritHandle
            wintypes.DWORD,  # dwProcessId
        )
        kernel32.VirtualAllocEx.restype = wintypes.LPVOID
        kernel32.VirtualAllocEx.argtypes = (
            wintypes.HANDLE,  # hProcess
            wintypes.LPVOID,  # lpAddress
            ctypes.c_size_t,  # dwSize
            wintypes.DWORD,   # flAllocationType
            wintypes.DWORD,   # flProtect
        )
        kernel32.VirtualFreeEx.restype = wintypes.BOOL
        kernel32.VirtualFreeEx.argtypes = (
            wintypes.HANDLE,  # hProcess
            wintypes.LPVOID,  # lpAddress
            ctypes.c_size_t,  # dwSize
            wintypes.DWORD,   # dwFreeType
        )
        kernel32.WriteProcessMemory.restype = wintypes.BOOL
        kernel32.WriteProcessMemory.argtypes = (
            wintypes.HANDLE,                  # hProcess
            wintypes.LPVOID,                  # lpBaseAddress
            wintypes.LPCVOID,                 # lpBuffer
            ctypes.c_size_t,                  # nSize
            ctypes.POINTER(ctypes.c_size_t),  # lpNumberOfBytesWritten _Out_
        )
        kernel32.CreateRemoteThread.restype = wintypes.LPVOID
        kernel32.CreateRemoteThread.argtypes = (
            wintypes.HANDLE,         # hProcess
            wintypes.LPVOID,         # lpThreadAttributes
            ctypes.c_size_t,         # dwStackSize
            wintypes.LPVOID,         # lpStartAddress
            wintypes.LPVOID,         # lpParameter
            wintypes.DWORD,          # dwCreationFlags
            wintypes.LPDWORD,        # lpThreadId _Out_
        )
        kernel32.WaitForSingleObject.restype = wintypes.DWORD
        kernel32.WaitForSingleObject.argtypes = (
            wintypes.HANDLE,  # hHandle
            wintypes.DWORD,   # dwMilliseconds
        )
        kernel32.GetExitCodeThread.restype = wintypes.BOOL
        kernel32.GetExitCodeThread.argtypes = (
            wintypes.HANDLE,   # hThread
            wintypes.LPDWORD,  # lpExitCode
        )
        kernel32.CloseHandle.restype = wintypes.BOOL
        kernel32.CloseHandle.argtypes = (
            wintypes.HANDLE,  # hObject
        )

        # Start the injection.
        size = (len(dll_path) + 1) * ctypes.sizeof(wintypes.WCHAR)
        h_process = None
        adr_path = None
        h_thread = None
        try:
            # Get handle to the running process.
            h_process = kernel32.OpenProcess(
                PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
                PROCESS_VM_WRITE, False, pid)
            if h_process is None:
                self._raise_windows_error("OpenProcess()", h_process)

            # Allocate memory for the DLL path.
            adr_path = kernel32.VirtualAllocEx(
                h_process, None, size, MEM_COMMIT | MEM_RESERVE,
                PAGE_READWRITE)
            if adr_path is None:
                self._raise_windows_error("VirtualAllocEx()", adr_path)

            # Write DLL path into the allocated memory region.
            success = kernel32.WriteProcessMemory(
                h_process, adr_path, dll_path, size, None)
            if not success:
                self._raise_windows_error("WriteProcessMemory()", success)

            # Create and start new thread in the process. The entry point of
            # the new thread is LoadLibraryW() with our DLL path as argument.
            h_thread = kernel32.CreateRemoteThread(
                h_process, None, 0, kernel32.LoadLibraryW, adr_path, 0, None)
            if h_thread is None:
                self._raise_windows_error("CreateRemoteThread()", h_thread)

            # Wait (with 10s timeout) until the thread exited, i.e. our DLL
            # injection either succeeded or failed.
            error = kernel32.WaitForSingleObject(h_thread, 10000)
            if error:
                self._raise_windows_error("WaitForSingleObject()", error)

            # Get the exit code of our thread, which corresponds to the return
            # value of LoadLibraryW() so we can check if the DLL was loaded
            # successfully or not.
            libfunq_handle = wintypes.DWORD(0)
            success = kernel32.GetExitCodeThread(
                h_thread, byref(libfunq_handle))
            if not success:
                self._raise_windows_error("GetExitCodeThread()", success)
            if not libfunq_handle:
                self._raise_windows_error("LoadLibraryW()", libfunq_handle)
        finally:
            if adr_path is not None:
                success = kernel32.VirtualFreeEx(h_process, adr_path, 0,
                                                 MEM_RELEASE)
                if not success:
                    self._raise_windows_error("VirtualFreeEx()", success)

            if h_thread is not None:
                success = kernel32.CloseHandle(h_thread)
                if not success:
                    self._raise_windows_error("CloseHandle()", success)

            if h_process is not None:
                success = kernel32.CloseHandle(h_process)
                if not success:
                    self._raise_windows_error("CloseHandle()", success)

    def _raise_windows_error(self, function_name, return_value):
        """
        Helper function to raise an error returned by a WIN32 API function.
        """
        last_error = ctypes.get_last_error()
        win_error = ctypes.WinError(last_error)
        message = "Failed to inject DLL! "
        message += "{} returned 0x{:X}. ".format(function_name, return_value)
        message += "The last error is 0x{:X} ({}). ".format(
            last_error, str(win_error))
        message += "Maybe x86/x64 mismatch between python.exe and Qt DLLs?"
        raise RuntimeError(message)
