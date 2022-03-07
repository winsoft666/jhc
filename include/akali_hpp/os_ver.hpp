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

#ifndef AKALI_OS_VER_HPP__
#define AKALI_OS_VER_HPP__

#include "akali_hpp/arch.hpp"

#ifdef AKALI_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <strsafe.h>
#endif
#elif defined(AKALI_MACOS)
#include <sys/sysctl.h>
#elif defined(AKALI_LINUX)
#include <sys/stat.h>
#include <sys/utsname.h>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <regex>
#endif

#pragma warning(disable : 4996)

namespace akl {
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

    static bool IsWindowsVistaOrHigher() {
        const WinVerInfo wvi = GetWinVer();
        return (wvi.major >= 6);
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

        return !!bIsWow64;
    }
#endif

    static std::string GetOSVersion() {
#ifdef AKALI_WIN
        const WinVerInfo wvi = GetWinVer();
        char result[100] = {0};
        StringCchPrintfA(result, 100, "%d.%d.%d-%d", wvi.major, wvi.minor, wvi.build, wvi.productType);
        return result;
#elif defined(AKALI_MACOS)
        char result[1024] = {0};
        size_t size = sizeof(result);
        if (sysctlbyname("kern.osrelease", result, &size, nullptr, 0) == 0)
            return result;
        return "<apple>";

#elif defined(AKALI_LINUX)
        static std::regex pattern("DISTRIB_DESCRIPTION=\"(.*)\"");

        std::string line;
        std::ifstream stream("/etc/lsb-release");
        while (getline(stream, line)) {
            std::smatch matches;
            if (std::regex_match(line, matches, pattern))
                return matches[1];
        }

        return "<linux>";
#endif
    }
};
}  // namespace akl
#endif  // !AKALI_OS_VER_HPP__
