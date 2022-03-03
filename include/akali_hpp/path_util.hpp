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
typedef std::wstring PathString;
#define PATH_FORMAT_SPECIFIER "ls"
#else
typedef std::string PathString;
#define PATH_FORMAT_SPECIFIER "s"
#endif  // !AKALI_WIN

typedef PathString::value_type PathChar;

class PathUtil {
   public:
    static const PathChar kEndChar;

    // Null-terminated array of separators used to separate components in path.
    // Each character in this array is a valid separator
    static const std::vector<PathChar> kFilePathSeparators;

    // A special path component meaning "this directory."
    static const std::vector<PathString> kFilePathCurrentDirectory;

    // A special path component meaning "the parent directory."
    static const std::vector<PathString> kFilePathParentDirectory;

    // The character used to identify a file extension.
    static const PathChar kFilePathExtensionSeparator;

    static bool IsPathSeparator(const PathChar c) {
        if (c == kEndChar)
            return false;

        const size_t len = kFilePathSeparators.size();
        for (size_t i = 0; i < len; i++) {
            if (c == kFilePathSeparators[i])
                return true;
        }

        return false;
    }

    static bool IsPathSeparator(const PathString& s) {
        if (s.empty())
            return false;

        const PathChar c = s[0];
        return IsPathSeparator(c);
    }

#ifdef AKALI_WIN
    static PathString GetWindowsFolder() {
        PathChar szBuf[MAX_PATH] = {0};
        const DWORD result = ::GetWindowsDirectoryW(szBuf, MAX_PATH);
        if (result == 0)
            return PathString();

        PathString tempPath = szBuf;
        if (!tempPath.empty())
            if (tempPath.back() != L'\\')
                tempPath.push_back(L'\\');
        return tempPath;
    }

    static PathString GetSystemFolder() {
        PathChar szBuf[MAX_PATH] = {0};
        const DWORD result = ::GetSystemDirectoryW(szBuf, MAX_PATH);
        if (result == 0)
            return PathString();

        PathString tempPath = szBuf;
        if (!tempPath.empty())
            if (tempPath.back() != L'\\')
                tempPath.push_back(L'\\');
        return tempPath;
    }

    static PathString GetTempFolder() {
        PathChar szBuf[MAX_PATH] = {0};
        const DWORD result = ::GetTempPathW(MAX_PATH, szBuf);
        if (result == 0)
            return PathString();

        PathString tempPath = szBuf;
        if (!tempPath.empty())
            if (tempPath.back() != L'\\')
                tempPath.push_back(L'\\');
        return tempPath;
    }

    static PathString GetLocalAppDataFolder() {
#if (NTDDI_VERSION < NTDDI_VISTA)
#ifndef KF_FLAG_CREATE
#define KF_FLAG_CREATE 0x00008000
#endif
#endif
        PathString tempPath;
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
            PathChar buffer[MAX_PATH] = {0};
            if (S_OK == ::SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, buffer))
                tempPath = buffer;
        }

        if (!tempPath.empty())
            if (tempPath.back() != L'\\')
                tempPath.push_back(L'\\');

        return tempPath;
    }
#else
#endif
};

#ifdef AKALI_WIN
const PathChar PathUtil::kEndChar = L'\0';
const std::vector<PathChar> PathUtil::kFilePathSeparators = {L'\\', L'/'};
const std::vector<PathString> PathUtil::kFilePathCurrentDirectory = {L"."};
const std::vector<PathString> PathUtil::kFilePathParentDirectory = {L".."};
const PathChar PathUtil::kFilePathExtensionSeparator = L'.';
#else
const PathChar PathUtil::kEndChar = '\0';
const std::vector<PathChar> PathUtil::kFilePathSeparators = {"/"};
const std::vector<PathString> PathUtil::kFilePathCurrentDirectory = {"."};
const std::vector<PathString> PathUtil::kFilePathParentDirectory = {".."};
const PathChar PathUtil::kFilePathExtensionSeparator = '.';
#endif  // AKALI_WIN
}  // namespace akali_hpp

#endif  // !AKALI_PATH_UTIL_HPP_