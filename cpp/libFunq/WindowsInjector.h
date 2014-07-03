#ifndef WINDOWSINJECTOR_H
#define WINDOWSINJECTOR_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern "C" int __stdcall DllMain( HINSTANCE , DWORD fdwReason, LPVOID  );

#endif // WINDOWSINJECTOR_H
