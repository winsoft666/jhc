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

#ifndef AKALI_PATH_UTIL_HPP_
#define AKALI_PATH_UTIL_HPP_

#include <string>
#include <vector>
#include "akali_hpp/arch.hpp"
#ifdef AKALI_WIN
#include <shlobj_core.h>
#endif
#include "akali_hpp/os_ver.hpp"

namespace akali_hpp {
#ifdef AKALI_WIN
#define PATH_FS "ls"
#else
#define PATH_FS "s"
#endif  // !AKALI_WIN

class PathUtil {
   public:
#ifdef AKALI_WIN
    static std::wstring GetWindowsFolder() {
        wchar_t szBuf[MAX_PATH] = {0};
        const DWORD result = ::GetWindowsDirectoryW(szBuf, MAX_PATH);
        if (result == 0)
            return filesystem::path::string_type();

        filesystem::path::string_type tempPath = szBuf;
        if (!tempPath.empty())
            if (tempPath.back() != L'\\')
                tempPath.push_back(L'\\');
        return tempPath;
    }

    static std::wstring GetSystemFolder() {
        wchar_t szBuf[MAX_PATH] = {0};
        const DWORD result = ::GetSystemDirectoryW(szBuf, MAX_PATH);
        if (result == 0)
            return std::wstring();

        std::wstring tempPath = szBuf;
        if (!tempPath.empty())
            if (tempPath.back() != L'\\')
                tempPath.push_back(L'\\');
        return tempPath;
    }

    static std::wstring GetTempFolder() {
        wchar_t szBuf[MAX_PATH] = {0};
        const DWORD result = ::GetTempPathW(MAX_PATH, szBuf);
        if (result == 0)
            return std::wstring();

        std::wstring tempPath = szBuf;
        if (!tempPath.empty())
            if (tempPath.back() != L'\\')
                tempPath.push_back(L'\\');
        return tempPath;
    }

    static std::wstring GetLocalAppDataFolder() {
#if (NTDDI_VERSION < NTDDI_VISTA)
#ifndef KF_FLAG_CREATE
#define KF_FLAG_CREATE 0x00008000
#endif
#endif
        std::wstring tempPath;
        if (OSVersion::IsWindowsVistaOrHigher()) {
            typedef HRESULT(WINAPI * __SHGetKnownFolderPath)(REFKNOWNFOLDERID, DWORD, HANDLE, PWSTR*);
            HMODULE hDll = ::LoadLibraryW(L"shell32.dll");
            if (hDll != NULL) {
                __SHGetKnownFolderPath _SHGetKnownFolderPath =
                    reinterpret_cast<__SHGetKnownFolderPath>(::GetProcAddress(hDll, "SHGetKnownFolderPath"));
                if (_SHGetKnownFolderPath != NULL) {
                    PWSTR result = NULL;
                    if (S_OK == _SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, NULL, &result)) {
                        tempPath = result;
                        ::CoTaskMemFree(result);
                    }
                }
                ::FreeLibrary(hDll);
            }
        }
        else {
            // On Windows XP, CSIDL_LOCAL_APPDATA represents "{user}\Local Settings\Application Data"
            // while CSIDL_APPDATA represents "{user}\Application Data"
            wchar_t buffer[MAX_PATH] = {0};
            if (S_OK == ::SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, buffer))
                tempPath = buffer;
        }

        if (!tempPath.empty())
            if (tempPath.back() != L'\\')
                tempPath.push_back(L'\\');

        return tempPath;
    }
#endif
};

}  // namespace akali_hpp

#endif  // !AKALI_PATH_UTIL_HPP_