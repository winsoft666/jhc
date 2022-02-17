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

#ifndef AKALI_OS_VER_HPP__
#define AKALI_OS_VER_HPP__

#include "akali_hpp/arch.hpp"

#ifdef AKALI_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#pragma warning(disable : 4996)

namespace akali_hpp {
/*
Operating system                            dwMajorVersion     dwMinorVersion         dwBuild                      Other
Windows 11                                       10               0                   >=22000         Not applicable     
Windows 10 Technical Preview                     10               0                                   OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server Technical Preview                 10               0                                   OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows 8.1                                      6                3                                   OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2012 R2                           6                3                                   OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows 8                                        6                2                                   OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2012                              6                2                                   OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows 7                                        6                1                                   OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2008 R2                           6                1                                   OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows Server 2008                              6                0                                   OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows Vista                                    6                0                                   OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2003 R2                           5                2                                   GetSystemMetrics(SM_SERVERR2) != 0
Windows Home Server                              5                2                                   OSVERSIONINFOEX.wSuiteMask & VER_SUITE_WH_SERVER
Windows Server 2003                              5                2                                   GetSystemMetrics(SM_SERVERR2) == 0
Windows XP Professional x64 Edition              5                2                                   (OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION) && (SYSTEM_INFO.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
Windows XP                                       5                1                                   Not applicable
Windows 2000                                     5                0                                   Not applicable
*/

typedef struct _WinVerInfo {
    int major;
    int minor;
    int build;
    int productType;

    _WinVerInfo() {
        major = minor = build = productType = -1;
    }
} WinVerInfo;

class OSVersion {
   public:
#ifdef AKALI_WIN
    static WinVerInfo GetWinVer() {
        WinVerInfo wvf;
        LONG(WINAPI * RtlGetVersion)
        (LPOSVERSIONINFOEX);
        *(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

        OSVERSIONINFOEX osversion;
        osversion.dwOSVersionInfoSize = sizeof(osversion);
        osversion.szCSDVersion[0] = TEXT('\0');

        if (RtlGetVersion != NULL) {
            // RtlGetVersion uses 0 (STATUS_SUCCESS) as return value when succeeding
            if (RtlGetVersion(&osversion) != 0)
                return wvf;
        }
        else {
            // GetVersionEx was deprecated in Windows 10, only use it as fallback
            OSVERSIONINFOEX osversion;
            osversion.dwOSVersionInfoSize = sizeof(osversion);
            osversion.szCSDVersion[0] = TEXT('\0');
            if (!GetVersionEx((LPOSVERSIONINFO)&osversion))
                return wvf;
        }

        wvf.major = (int)osversion.dwMajorVersion;
        wvf.minor = (int)osversion.dwMinorVersion;
        wvf.build = (int)osversion.dwBuildNumber;

        DWORD dwProductType = 0;
        if (GetProductInfo(osversion.dwMajorVersion, osversion.dwMinorVersion, 0, 0, &dwProductType)) {
            wvf.productType = (int)dwProductType;
        }
        return wvf;
    }

    static bool IsWindows11() {
        const WinVerInfo wvi = GetWinVer();
        return (wvi.major == 10 && wvi.minor == 0 && wvi.build >= 22000);
    }

    static bool IsWin64() {
        typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
        static LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
        BOOL bIsWow64 = FALSE;

        if (NULL == fnIsWow64Process) {
            HMODULE h = GetModuleHandleW(L"kernel32");
            if (h)
                fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(h, "IsWow64Process");
        }

        if (NULL != fnIsWow64Process) {
            fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
        }

        return bIsWow64 == 1;
    }
#endif
};
}  // namespace akali_hpp
#endif  // !AKALI_OS_VER_HPP__
