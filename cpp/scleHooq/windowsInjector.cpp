#include "windowsInjector.h"
#include <scleHook.h>
#include <QDebug>

static bool once = false;

LRESULT CALLBACK dummyHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (!once) {
        ScleHooq::activation(false);
        once=true;
    }
    return ::CallNextHookEx(NULL, nCode, wParam, lParam);
}

void installHooq(HINSTANCE hMod, DWORD dwThreadId)
{
    SetWindowsHookEx(WH_GETMESSAGE, dummyHook, hMod, dwThreadId);
}
