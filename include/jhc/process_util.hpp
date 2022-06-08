/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022 JiangXueqiao <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef JHC_PROCESS_UTIL_H__
#define JHC_PROCESS_UTIL_H__

#include "jhc/arch.hpp"
#include <string>
#include <list>

#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif  // !_INC_WINDOWS
#include <tchar.h>
#include <strsafe.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#elif defined(_GNU_SOURCE)
#include <errno.h>
#endif
#include "jhc/string_encode.hpp"
#include "jhc/filesystem.hpp"

#pragma warning(disable : 4996)

namespace jhc {
class ProcessUtil {
   public:
#ifdef JHC_WIN
    static bool IsRunAsAdminPrivilege(HANDLE hProcess) {
        BOOL fRet = FALSE;
        HANDLE hToken = NULL;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
            TOKEN_ELEVATION Elevation;
            DWORD cbSize = sizeof(TOKEN_ELEVATION);
            if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
                fRet = (Elevation.TokenIsElevated != 0);
            }
        }

        if (hToken) {
            CloseHandle(hToken);
        }

        return !!fRet;
    }

    static bool IsRunAsAdminPrivilege(DWORD dwPid) {
        HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
        if (!hProcess)
            return false;
        const bool ret = IsRunAsAdminPrivilege(hProcess);
        CloseHandle(hProcess);
        return ret;
    }

    static bool SetUIPIMsgFilter(HWND hWnd, UINT uMessageID, BOOL bAllow) {
        OSVERSIONINFO VersionTmp;
        VersionTmp.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionEx(&VersionTmp);
        BOOL res = FALSE;

        if (VersionTmp.dwMajorVersion >= 6) {  // vista above.
            BOOL(WINAPI * pfnChangeMessageFilterEx)
            (HWND, UINT, DWORD, PCHANGEFILTERSTRUCT);
            BOOL(WINAPI * pfnChangeMessageFilter)
            (UINT, DWORD);

            CHANGEFILTERSTRUCT filterStatus;
            filterStatus.cbSize = sizeof(CHANGEFILTERSTRUCT);

            HINSTANCE hlib = LoadLibrary(_T("user32.dll"));
            if (hlib) {
                (FARPROC&)pfnChangeMessageFilterEx = GetProcAddress(hlib, "ChangeWindowMessageFilterEx");

                if (pfnChangeMessageFilterEx != NULL && hWnd != NULL) {
                    res = pfnChangeMessageFilterEx(hWnd, uMessageID, (bAllow ? MSGFLT_ADD : MSGFLT_REMOVE),
                                                   &filterStatus);
                }

                // If failed, try again.
                if (!res) {
                    (FARPROC&)pfnChangeMessageFilter = GetProcAddress(hlib, "ChangeWindowMessageFilter");

                    if (pfnChangeMessageFilter != NULL) {
                        res = pfnChangeMessageFilter(uMessageID, (bAllow ? MSGFLT_ADD : MSGFLT_REMOVE));
                    }
                }

                FreeLibrary(hlib);
            }
        }
        else {
            res = TRUE;
        }

        return !!res;
    }

    static bool CreateNewProcess(const std::wstring& path, const std::wstring& param) {
        WCHAR szDir[MAX_PATH] = {0};
        StringCchPrintfW(szDir, MAX_PATH, L"%s", path.c_str());
        PathRemoveFileSpecW(szDir);

        WCHAR szFullCMD[1024];
        if (param.length() > 0)
            StringCchPrintfW(szFullCMD, 1024, L"\"%s\" %s", path.c_str(), param.c_str());
        else
            StringCchPrintfW(szFullCMD, 1024, L"\"%s\"", path.c_str());

        STARTUPINFOW si = {sizeof(si)};
        PROCESS_INFORMATION pi = {0};
        if (!CreateProcessW(NULL, szFullCMD, NULL, NULL, FALSE, 0, NULL, szDir, &si, &pi))
            return false;

        if (pi.hThread)
            CloseHandle(pi.hThread);

        if (pi.hProcess)
            CloseHandle(pi.hProcess);

        return true;
    }

#endif

    // On windows, path is encoded by ANSI, otherwise, is UTF8.
    //
    static std::string GetCurrentExePath() {
        std::string ret;
#ifdef JHC_WIN
        char* pBuf = NULL;
        DWORD dwBufSize = MAX_PATH;

        do {
            pBuf = (char*)malloc(dwBufSize);
            if (!pBuf)
                break;
            memset(pBuf, 0, dwBufSize);

            DWORD dwGot = GetModuleFileNameA(NULL, pBuf, dwBufSize);
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                free(pBuf);
                dwBufSize *= 2;
            }
            else {
                break;
            }
        } while (true);

        if (pBuf) {
            ret = pBuf;
            free(pBuf);
        }
#elif defined(JHC_MACOS)
        return getprogname();
#elif defined(_GNU_SOURCE)
        return program_invocation_name;
#endif
        return ret;
    }

    // On windows, directory is encoded by ANSI, otherwise, is UTF8.
    //
    static std::string GetCurrentExeDirectory() {
        const std::string path = GetCurrentExePath();
#ifdef JHC_WIN
        const std::wstring pathW = StringEncode::AnsiToUnicode(path);
        fs::path p(pathW);
        p.remove_filename();
        return StringEncode::UnicodeToAnsi(p.wstring());
#else
        fs::path p(path);
        p.remove_filename();
        return p.u8string();
#endif
    }

#ifdef JHC_WIN
    static BOOL CALLBACK EnumResourceNameCallback(HMODULE hModule, LPCWSTR lpType, LPWSTR lpName, LONG_PTR lParam) {
        std::list<std::string>* pList = (std::list<std::string>*)lParam;

        HRSRC hResInfo = FindResource(hModule, lpName, lpType);
        DWORD cbResource = SizeofResource(hModule, hResInfo);

        HGLOBAL hResData = LoadResource(hModule, hResInfo);
        if (hResData) {
            const BYTE* pResource = (const BYTE*)LockResource(hResData);

            if (pResource) {
                std::string strU8;
                strU8.assign((const char*)pResource, cbResource);
                pList->push_back(strU8);
            }

            UnlockResource(hResData);
            FreeResource(hResData);
        }

        return TRUE;  // Keep going
    }

    static bool GetExeOrDllManifest(const std::wstring& path, std::list<std::string>& manifests) {
        HMODULE hModule = LoadLibraryExW(path.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
        if (!hModule)
            return false;

        EnumResourceNamesW(hModule, RT_MANIFEST, EnumResourceNameCallback, (LONG_PTR)&manifests);
        FreeLibrary(hModule);

        return true;
    }
#endif
};
}  // namespace jhc

#endif
