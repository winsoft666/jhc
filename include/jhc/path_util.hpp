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

#ifndef JHC_PATH_UTIL_HPP_
#define JHC_PATH_UTIL_HPP_
#include "jhc/config.hpp"
#include <string>
#include <vector>
#include "jhc/arch.hpp"

namespace jhc {
#ifdef JHC_WIN
#define PATH_FS "ls"
#else
#define PATH_FS "s"
#endif  // !JHC_WIN

class PathUtil {
   public:
#ifdef JHC_WIN
    static std::wstring GetWindowsFolder();

    static std::wstring GetSystemFolder();

    static std::wstring GetTempFolder();

    static std::wstring GetLocalAppDataFolder();

    static bool OpenWinExplorerAndLocate(const std::wstring& path);

    static std::wstring GetWinExplorerDisplayName(const std::wstring& path);

    static std::string ReplaceKnownEnvToWow6432(const std::string& src);
    static std::wstring ReplaceKnownEnvToWow6432(const std::wstring& src);

    static std::string ExpandEnvString(const std::string& src, bool disableWow64FsRedirection = true);
    static std::wstring ExpandEnvString(const std::wstring& src, bool disableWow64FsRedirection = true);
#endif
};

}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/path_util.cc"
#endif
#endif  // !JHC_PATH_UTIL_HPP_
