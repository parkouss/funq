#include "WindowsInjector.h"
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
