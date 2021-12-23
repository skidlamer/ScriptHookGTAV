// ScriptHook.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "main.h"

void scriptRegister(HMODULE module, void(*function)())
{
	LOG_MESSAGE("HELLO", "From ScriptHookV");
	//g_ScriptThread.AddScript(module, function);
}

void scriptRegisterAdditionalThread(HMODULE module, void(*function)())
{
	//g_AdditionalThread.AddScript(module, function);
}

void scriptUnregister(HMODULE module)
{
	//g_AdditionalThread.RemoveScript(module);
	//g_ScriptThread.RemoveScript(module);
}
