#pragma once

namespace utils 
{
    namespace string 
    {
        template <typename T>
        constexpr std::basic_string<T> vaFormat(const T* format, va_list args)
        {
            int size;

            std::basic_string<T> s;

            if constexpr (std::is_same_v<T, char>)
                size = vsnprintf(nullptr, 0, format, args);
            else
                size = _vsnwprintf(nullptr, 0, format, args);

            size++; // Zero termination

            s.resize(size);

            if constexpr (std::is_same_v<T, char>)
                vsnprintf(&s[0], size, format, args);
            else
                _vsnwprintf(&s[0], size, format, args);

            return s;
        }

        //
        //  toLowercase string
        //
        template <typename T>
        constexpr std::basic_string<T> lowercase(const std::basic_string<T>& s)
        {
            std::basic_string<T> s2 = s;
            std::transform(s2.begin(), s2.end(), s2.begin(), tolower);
            return std::move(s2);
        }

        //
        // toUppercase string
        //
        template <typename T>
        constexpr std::basic_string<T> uppercase(const std::basic_string<T>& s)
        {
            std::basic_string<T> s2 = s;
            std::transform(s2.begin(), s2.end(), s2.begin(), toupper);
            return std::move(s2);
        }

    }

    namespace hash 
    {
        constexpr std::uint32_t joaat(const char* string, std::size_t length) {
            std::uint32_t hash = 0;
            for (std::size_t i = 0; i < length; ++i) {
                char ascii = string[i];
                ascii = (ascii >= 'A' && ascii <= 'Z') ? ascii + ('a' - 'A') : ascii;
                hash += ascii;
                hash += (hash << 10);
                hash ^= (hash >> 6);
            }
            hash += (hash << 3);
            hash ^= (hash >> 11);
            hash += (hash << 15);
            return hash;
        }
    }

    namespace handle
    {
        inline DWORD getProcessID(std::string process_name) {
            std::wstring process_name_wstr(process_name.begin(), process_name.end());
            LPCTSTR lpcszFileName = process_name_wstr.c_str();
            LPDWORD lpdwProcessIds;
            LPTSTR  lpszBaseName;
            HANDLE  hProcess;
            DWORD   i, cdwProcesses, dwProcessId = 0;

            lpdwProcessIds = (LPDWORD)HeapAlloc(GetProcessHeap(), 0, MAX_PROCESSES * sizeof(DWORD));
            if (lpdwProcessIds != NULL)
            {
                if (EnumProcesses(lpdwProcessIds, MAX_PROCESSES * sizeof(DWORD), &cdwProcesses))
                {
                    lpszBaseName = (LPTSTR)HeapAlloc(GetProcessHeap(), 0, MAX_PATH * sizeof(TCHAR));
                    if (lpszBaseName != NULL)
                    {
                        cdwProcesses /= sizeof(DWORD);
                        for (i = 0; i < cdwProcesses; i++)
                        {
                            hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, lpdwProcessIds[i]);
                            if (hProcess != NULL)
                            {
                                if (GetModuleBaseName(hProcess, NULL, lpszBaseName, MAX_PATH) > 0)
                                {
                                    if (!lstrcmpi(lpszBaseName, lpcszFileName))
                                    {
                                        dwProcessId = lpdwProcessIds[i];
                                        CloseHandle(hProcess);
                                        break;
                                    }
                                }
                                CloseHandle(hProcess);
                            }
                        }
                        HeapFree(GetProcessHeap(), 0, (LPVOID)lpszBaseName);
                    }
                }
                HeapFree(GetProcessHeap(), 0, (LPVOID)lpdwProcessIds);
            }
            return dwProcessId;
        }

        inline HMODULE loadLibrarySystem(const std::string& library_name, std::string* out_path) {
            std::unique_ptr<char[]> system_directory(new char[MAX_PATH]);
            GetSystemDirectoryA(system_directory.get(), MAX_PATH);

            fs::path path(system_directory.get());

            path.append(library_name);

            std::string lib_path(path.generic_string());

            if (out_path)
                *out_path = lib_path;
            
            return LoadLibraryA(lib_path.c_str());
        }

        inline HMODULE& getCurrentModule() {
            static HMODULE hModule = 0;
            if (!hModule)
                GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)&hModule, &hModule);
            return hModule;
        }

        inline bool getModulePath(std::string* out, HMODULE hModule) {
            std::unique_ptr<char[]> buffer(new char[MAX_PATH]);
            GetModuleFileNameA(hModule, buffer.get(), MAX_PATH);
            *out = buffer.get();
            return !out->empty();
        }

        inline bool getModuleDirectory(std::string* out, HMODULE hModule) {
            std::unique_ptr<char[]> buffer(new char[MAX_PATH]);
            GetModuleFileNameA(hModule, buffer.get(), MAX_PATH);
            PathRemoveFileSpecA(buffer.get());
            *out = buffer.get();
            return !out->empty();
        }
    }

    namespace path {
        inline bool stringPathAppend(std::string* path, const std::string& more) {
            std::unique_ptr<char[]> buffer(new char[MAX_PATH]);
            *path = PathCombineA(buffer.get(), path->c_str(), more.c_str());
            return !path->empty();
        }

        inline bool stringPathCombine(std::string* dest, const std::string& path, const std::string& more) {
            std::unique_ptr<char[]> buffer(new char[MAX_PATH]);
            *dest = PathCombineA(buffer.get(), path.c_str(), more.c_str());
            return !dest->empty();
        }

        inline bool doesFileExist(const std::string& path) {
            HANDLE hFile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                CloseHandle(hFile);
                return true;
            }
            return false;
        }

        inline bool getFullPathFromPath(std::string* path, const std::string& in_path) {
            if (PathIsRelativeA(in_path.c_str()))
            {
                std::unique_ptr<char[]> buffer(new char[MAX_PATH]);
                if (GetModuleFileNameA(handle::getCurrentModule(), buffer.get(), MAX_PATH) &&
                    PathRemoveFileSpecA(buffer.get()))
                {
                    PathAppendA(buffer.get(), in_path.c_str());
                    *path = buffer.get();
                }
            }
            else
            {
                *path = in_path;
            }

            if (doesFileExist(*path))
                return true;

            return false;
        }
    }
}

//
//  Formats string/wstring according to string format
//
template <typename T>
constexpr std::basic_string<T> fmt(const T* format, ...) {
    va_list args;
    va_start(args, format);
    return utils::string::vaFormat(format, args);
}

//
//  Jenkins's one_at_a_time hash function
//
constexpr std::uint32_t joaat(const char* string) {
    return utils::hash::joaat(string, std::strlen(string));
}

constexpr std::uint32_t operator""_joaat(const char* string, std::size_t length) {
    return utils::hash::joaat(string, length);
}
