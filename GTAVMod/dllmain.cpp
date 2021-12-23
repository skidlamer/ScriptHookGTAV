// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#pragma comment(lib, "..\\Release\\Lib\\ScriptHookGTAV.lib")
#include "..\Release\Lib\main.h"

void scriptMain() {

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  callReason, LPVOID lpReserved)
{
    switch (callReason)
    {
    case DLL_PROCESS_ATTACH:
        scriptRegister(hModule, scriptMain);
        break;
    case DLL_THREAD_ATTACH: case DLL_THREAD_DETACH: break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

