#ifndef WINDOWSINJECTOR_H
#define WINDOWSINJECTOR_H

#include <QtCore/QtGlobal>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "funq.h"

extern "C" int __stdcall DllMain( HINSTANCE , DWORD fdwReason, LPVOID  ) {

    switch( fdwReason )
    {
        case DLL_PROCESS_ATTACH:
            Funq::activate();
            break;
        case DLL_PROCESS_DETACH:
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
    }


    return TRUE;
}

#endif // WINDOWSINJECTOR_H
