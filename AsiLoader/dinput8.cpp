#include "pch.h"
#include "dinput8.h"

extern "C" EXPORT HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter)
{
	ModuleLoader::Get().loadPlugins("", "asi");

	return ModuleLoader::Get().DirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}

extern "C" HRESULT WINAPI DllCanUnloadNow(void)
{
	return ModuleLoader::Get().DllCanUnloadNow();
}

extern "C" HRESULT WINAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR * ppv)
{
	return ModuleLoader::Get().DllGetClassObject(rclsid, riid, ppv);
}

extern "C" EXPORT HRESULT WINAPI DllRegisterServer()
{
	return ModuleLoader::Get().DllRegisterServer();
}

extern "C" EXPORT HRESULT WINAPI DllUnregisterServer()
{
	return ModuleLoader::Get().DllUnregisterServer();
}