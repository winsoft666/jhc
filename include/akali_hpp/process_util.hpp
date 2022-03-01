/*******************************************************************************
*    Copyright (C) <2022>  <winsoft666@outlook.com>.
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

#ifndef AKALI_PROCESS_UTIL_H__
#define AKALI_PROCESS_UTIL_H__

#include "akali_hpp/arch.hpp"
#include <string>

#ifdef AKALI_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif // !_INC_WINDOWS
#include <tchar.h>
#include <strsafe.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#elif defined(_GNU_SOURCE)
#include <errno.h>
#endif

#pragma warning(disable : 4996)

namespace akali_hpp {
class ProcessUtil {
   public:
#ifdef AKALI_WIN
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

    static std::string GetCurrentProcessPath() {
        std::string ret;
#ifdef AKALI_WIN
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
#elif defined(AKALI_MACOS)
        return getprogname();
#elif defined(_GNU_SOURCE)
        return program_invocation_name;
#endif
        return ret;
    }
};
}  // namespace akali_hpp

#endif