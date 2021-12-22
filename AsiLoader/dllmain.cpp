// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "dinput8.h"

HMODULE hDinput8;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  callReason, LPVOID lpReserved)
{
    switch (callReason)
    {
    case DLL_PROCESS_ATTACH: {
        const std::string fileName(fs::current_path().u8string() + "\\Loader.log");
        static std::ofstream logFile(fileName);
        logFile << "\tGTAV ASI LOADER (build - Dec 22 2021)" << std::endl << "\t\t(C) Skid Lamer 2021" << std::endl << std::endl;
        g_log = std::make_unique<Log>(logFile);
        DisableThreadLibraryCalls(hModule);
        hDinput8 = hModule;
    }
        break;
    case DLL_THREAD_ATTACH: case DLL_THREAD_DETACH: break;
    case DLL_PROCESS_DETACH:
        FreeLibrary(hDinput8);
        break;
    }
    return TRUE;
}

