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
#include "jhc/config.hpp"
#include "jhc/arch.hpp"
#include <string>
#include <list>

#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif  // !_INC_WINDOWS
#endif

namespace jhc {
class ProcessUtil {
   public:
#ifdef JHC_WIN
    static bool IsRunAsAdminPrivilege(HANDLE hProcess);

    static bool IsRunAsAdminPrivilege(DWORD dwPid);

    static bool SetUIPIMsgFilter(HWND hWnd, unsigned int uMessageID, bool bAllow);

    static bool CreateNewProcess(const std::wstring& path, const std::wstring& param, DWORD *dwPID, HANDLE* pProcess);

#endif

    // On windows, path is encoded by ANSI, otherwise, is UTF8.
    //
    static std::string GetCurrentExePath();

    // On windows, directory is encoded by ANSI, otherwise, is UTF8.
    //
    static std::string GetCurrentExeDirectory();

#ifdef JHC_WIN
    static BOOL CALLBACK EnumResourceNameCallback(HMODULE hModule, LPCWSTR lpType, LPWSTR lpName, LONG_PTR lParam);

    static bool GetExeOrDllManifest(const std::wstring& path, std::list<std::string>& manifests);
#endif
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/process_util.cc"
#endif

#endif
