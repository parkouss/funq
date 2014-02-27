#include "WinDll.h"
#include <scleHook.h>
#include <QDebug>

static HHOOK g_hook = 0;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if(ul_reason_for_call == DLL_PROCESS_ATTACH && g_hook)
    {
        // Increase reference count
        wchar_t path[MAX_PATH];
        ::GetModuleFileNameW(static_cast<HMODULE>(hModule), path, MAX_PATH);
        ::LoadLibrary(path);
        // Remove hook
        //
    }
    return TRUE;
}

LRESULT CALLBACK dummyHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (g_hook) {
        ::UnhookWindowsHookEx(g_hook);
    }
    ScleHooq::activation(false);
    return ::CallNextHookEx(NULL, nCode, wParam, lParam);
}

void installHooq(HINSTANCE hMod, DWORD dwThreadId)
{
    g_hook = ::SetWindowsHookEx(WH_CALLWNDPROC, dummyHook, hMod, dwThreadId);
    ::PostThreadMessage(dwThreadId, WM_NULL, 0, 0); // map the DLL in the other thread
}
