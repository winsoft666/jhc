/*******************************************************************************
 * Copyright (C) 2021 - 2026, winsoft666, <winsoft666@outlook.com>.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Expect bugs
 *
 * Please use and enjoy. Please let me know of any bugs/improvements
 * that you have found/implemented and I will fix/incorporate them into this
 * file.
 *******************************************************************************/

#ifndef AKALI_PROCESS_UTIL_H__
#define AKALI_PROCESS_UTIL_H__

#include <string>
#include <strsafe.h>
#include <tchar.h>
#include <Shlwapi.h>

#include "akali_hpp/arch.hpp"

#ifdef AKALI_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif
#include <tchar.h>
#endif

#pragma warning(disable : 4996)
#pragma comment(lib, "Shlwapi.lib")

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
};
}  // namespace akali_hpp

#endif