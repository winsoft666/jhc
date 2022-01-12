/*******************************************************************************
 * Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
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

#ifndef AKALI_OS_VER_H__
#define AKALI_OS_VER_H__
#pragma once

#include "akali_hpp/arch.hpp"

#ifdef AKALI_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <string>

#pragma warning(disable : 4996)

/*
Operating system                            dwMajorVersion   dwMinorVersion              Other
Windows 10 Technical Preview                     10               0                  OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server Technical Preview                 10               0                  OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows 8.1                                      6                3                  OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2012 R2                           6                3                  OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows 8                                        6                2                  OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2012                              6                2                  OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows 7                                        6                1                  OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2008 R2                           6                1                  OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows Server 2008                              6                0                  OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows Vista                                    6                0                  OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2003 R2                           5                2                  GetSystemMetrics(SM_SERVERR2) != 0
Windows Home Server                              5                2                  OSVERSIONINFOEX.wSuiteMask & VER_SUITE_WH_SERVER
Windows Server 2003                              5                2                  GetSystemMetrics(SM_SERVERR2) == 0
Windows XP Professional x64 Edition              5                2                  (OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION) && (SYSTEM_INFO.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
Windows XP                                       5                1                  Not applicable
Windows 2000                                     5                0                  Not applicable
*/

namespace akali_hpp {
#if (defined _WIN32 || defined WIN32)
enum WinVer {
    WIN_2000,
    WIN_XP,
    WIN_XP_PRO,
    WIN_2003,
    WIN_2003_R2,
    WIN_HOME,
    WIN_VISTA,
    WIN_2008,
    WIN_2008_R2,
    WIN_7,
    WIN_2012,
    WIN_8,
    WIN_2012_R2,
    WIN_8_1,
    WIN_2016,
    WIN_10,      // Threshold 1: Version 1507, Build 10240.
    WIN_10_TH2,  // Threshold 2: Version 1511, Build 10586.
    WIN_10_RS1,  // Redstone 1: Version 1607, Build 14393.
    WIN_10_RS2,  // Redstone 2: Version 1703, Build 15063.
    WIN_10_RS3,  // Redstone 3: Version 1709, Build 16299.
    WIN_LAST     // Indicates error condition.
};

class OSVersion {
   public:
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
};
#endif
}  // namespace akali_hpp
#endif
#endif  // !AKALI_OS_VER_H__
