// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "OverlaysTicker.h"
#include "UIEventWatcher.h"

OverlaysTicker* ticker = nullptr;
UIEventWatcher* watcher = nullptr;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Disable unnecessary calls
        DisableThreadLibraryCalls(hModule);
        ticker = new OverlaysTicker();
        watcher = new UIEventWatcher(ticker);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

