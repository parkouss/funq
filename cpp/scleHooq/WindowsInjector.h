#ifndef WINDOWSINJECTOR_H
#define WINDOWSINJECTOR_H

#include <QtCore/QtGlobal>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

__declspec(dllexport) LRESULT CALLBACK dummyHook(int nCode, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllexport) void installHooq(HINSTANCE hMod, DWORD dwThreadId);

#endif // WINDOWSINJECTOR_H
