#pragma once

#include "NonCopyable.h"

typedef const IMAGE_NT_HEADERS64 NT64H;
typedef std::vector<char> bufferVec;

class ModuleLoader : NonCopyable
{

public:
	HRESULT(WINAPI* DirectInput8Create)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter);
	HRESULT(WINAPI* DllCanUnloadNow)(void);
	HRESULT(WINAPI* DllGetClassObject)(REFCLSID rclsid, REFIID  riid, LPVOID* ppv);
	HRESULT(WINAPI* DllRegisterServer)();
	HRESULT(WINAPI* DllUnregisterServer)();

	ModuleLoader()
	{
		std::string loaded_module_path;
		std::string module_filename;

		m_module = utils::handle::loadLibrarySystem("dinput8.dll", &loaded_module_path);

		if (!m_module)
		{
			HRESULT hr = GetLastError();
			std::unique_ptr<char[]> error_msg(new char[MAX_PATH]);
			sprintf_s(error_msg.get(), MAX_PATH, "Cannot load \"%s\" error: 0x%x", loaded_module_path.c_str(), hr);
			LOG_ERROR(error_msg.get());
			MessageBoxA(NULL, error_msg.get(), "Error", MB_ICONERROR);
			exit(hr);
		}
		else
		{
			LOG_PRINT("Loaded \"%s\"", loaded_module_path.c_str());
		}

		GetProcAddress("DirectInput8Create", &DirectInput8Create);
		GetProcAddress("DllCanUnloadNow", &DllCanUnloadNow);
		GetProcAddress("DllGetClassObject", &DllGetClassObject);
		GetProcAddress("DllRegisterServer", &DllRegisterServer);
		GetProcAddress("DllUnregisterServer", &DllUnregisterServer);
	}

	bool LoadModule(const std::string& path) {

		filePath = path;

		std::ifstream inputFile(path, std::ios::binary);
		if (inputFile.fail()) {
			return false;
		}
		std::vector<char> bufferTemp((std::istreambuf_iterator<char>(inputFile)), (std::istreambuf_iterator<char>()));
		fileBuffer.swap(bufferTemp);

		bool parseSuccess = ParsePE();
		if (!parseSuccess) {
			return false;
		}

		return true;
	}

	bool ParsePE() {

		// Get DOS header
		const IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(fileBuffer.data());

		// Not a valid PE
		if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
			return false;
		}

		// Get NT header
		ntHeader = reinterpret_cast<const IMAGE_NT_HEADERS64*>(reinterpret_cast<const uint8_t*>(dosHeader) + dosHeader->e_lfanew);

		return true;
	}

	uint64_t GetDirectoryAddress(int index) {

		const IMAGE_DATA_DIRECTORY* dataDirectory = ntHeader->OptionalHeader.DataDirectory;

		return RVAToVA(dataDirectory[index].VirtualAddress);
	}

	uint64_t RVAToVA(uint32_t rva) const {

		const IMAGE_SECTION_HEADER* sectionHeader = reinterpret_cast<const IMAGE_SECTION_HEADER*>(ntHeader + 1);
		for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i, ++sectionHeader) {

			if (rva >= sectionHeader->VirtualAddress && rva <= sectionHeader->VirtualAddress + sectionHeader->Misc.VirtualSize) {

				return reinterpret_cast<uint64_t>(fileBuffer.data()) + (rva - sectionHeader->VirtualAddress + sectionHeader->PointerToRawData);
			}
		}

		return 0;
	}

	void loadPlugins(std::string folder, std::string exten)
	{
		std::string currentFolder;
		utils::handle::getModuleDirectory(&currentFolder, NULL);

		std::string pluginFolder;
		if (folder.empty())
		{
			pluginFolder = currentFolder;
			if (exten == "dll" || exten == "rpf" || exten == "exe" || exten == "txt")
			{
				LOG_ERROR("\tThe DLL file extension cannot be used in the current folder");
				return;
			}
		}
		else pluginFolder = currentFolder + "\\" + folder;

		const std::string searchQuery = pluginFolder + "\\*." + exten;

		WIN32_FIND_DATAA fileData;
		HANDLE fileHandle = FindFirstFileA(searchQuery.c_str(), &fileData);
		if (fileHandle != INVALID_HANDLE_VALUE) {

			do {

				const std::string pluginPath = pluginFolder + "\\" + fileData.cFileName;

				LOG_PRINT("Loading \"%s\"", pluginPath.c_str());

				if (!LoadModule(pluginPath)) {

					LOG_ERROR("\tFailed to load image");
					continue;
				}

				HMODULE module = LoadLibraryA(pluginPath.c_str());
				if (module) {
					LOG_PRINT("\tLoaded \"%s\" => 0x%p", fileData.cFileName, module);
					m_plugins[module] = fileData.cFileName;
				}
				else {
					LOG_ERROR("\tFailed to load");
				}

			} while (FindNextFileA(fileHandle, &fileData));

			FindClose(fileHandle);
		}

		LOG_PRINT("Finished loading *.%s plugins", exten.c_str());
	}

	void unloadPlugins()
	{
		for (auto& plugin : m_plugins)
		{
			LOG_PRINT("Unloading %s", plugin.second.c_str());
			FreeLibrary(plugin.first);
		}

		m_plugins.clear();
	}

	~ModuleLoader()
	{
		if (m_module)
		{
			unloadPlugins();
			std::string dinput_path;
			utils::handle::getModulePath(&dinput_path, m_module);
			LOG_PRINT("Unloading %s", dinput_path.c_str());
			FreeLibrary(m_module);
		}
	}

	static ModuleLoader& Get()
	{
		static ModuleLoader instance;
		return instance;
	}

private:
	template<typename T>
	inline void GetProcAddress(const char* funcname, T* ppfunc)
	{
		*ppfunc = reinterpret_cast<T>(::GetProcAddress(m_module, funcname));
	}

	HMODULE m_module;

	std::map<HMODULE, std::string> m_plugins;

	std::string		filePath;
	bufferVec		fileBuffer;
	NT64H* ntHeader = nullptr;
};
