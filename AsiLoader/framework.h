#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <Xinput.h>

#define MAX_PROCESSES 512

#define IMPORT __declspec(dllimport)

#define EXPORT __declspec(dllexport)

#define uniquePtr(ptr) std::unique_ptr<ptr, std::default_delete<ptr>>

#pragma comment(lib, "shlwapi.lib")
#include <Shlwapi.h>

#pragma comment(lib, "psapi.lib")
#include <psapi.h>

#include <filesystem>
namespace fs = std::filesystem;

