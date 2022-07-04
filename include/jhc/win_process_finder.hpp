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

#ifndef JHC_PROCESS_FINDER_HPP__
#define JHC_PROCESS_FINDER_HPP__
#pragma once

#include "jhc/config.hpp"
#include "jhc/arch.hpp"

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
#include <TlHelp32.h>
#endif

#include <string>
#include "jhc/macros.hpp"

namespace jhc {
#ifdef JHC_WIN
class WinProcessFinder {
   public:
    JHC_DISALLOW_COPY_MOVE(WinProcessFinder);

    // dwFlags can be one or more of the following values.
    // TH32CS_SNAPHEAPLIST
    // TH32CS_SNAPPROCESS
    // TH32CS_SNAPTHREAD
    // TH32CS_SNAPMODULE
    // TH32CS_SNAPMODULE32
    // TH32CS_INHERIT
    // TH32CS_SNAPALL = (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE)
    //
    explicit WinProcessFinder(DWORD dwFlags = TH32CS_SNAPALL, DWORD dwProcessID = 0);

    ~WinProcessFinder();

    bool createSnapShot(DWORD dwFlag, DWORD dwProcessID);

    bool processFirst(PPROCESSENTRY32 ppe) const;

    bool processNext(PPROCESSENTRY32 ppe) const;

    // Don't forgot pe.dwSize = sizeof(PROCESSENTRY32);
    bool processFind(DWORD dwProcessId, PPROCESSENTRY32 ppe) const;

    bool processFind(PCTSTR pszExeName, PPROCESSENTRY32 ppe, BOOL bExceptSelf = false) const;

    bool moduleFirst(PMODULEENTRY32 pme) const;

    bool moduleNext(PMODULEENTRY32 pme) const;

    bool moduleFind(PVOID pvBaseAddr, PMODULEENTRY32 pme) const;

    bool moduleFind(PTSTR pszModName, PMODULEENTRY32 pme) const;

    static bool IsExist(const std::wstring& processName);

    static bool IsExist(const std::string& processName);

    static bool IsExist(const std::wstring& processName, DWORD* dwPID);

    static bool IsExist(const std::string& processName, DWORD* dwPID);

   private:
    HANDLE m_hSnapShot;
};
#endif
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/win_process_finder.cc"
#endif

#endif
