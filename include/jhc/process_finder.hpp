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

#include "jhc/arch.hpp"

#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif // !_INC_WINDOWS
#include <TlHelp32.h>
#endif

#include <string>
#include "jhc/string_encode.hpp"
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
    explicit WinProcessFinder(DWORD dwFlags = TH32CS_SNAPALL, DWORD dwProcessID = 0) {
        m_hSnapShot = INVALID_HANDLE_VALUE;
        createSnapShot(dwFlags, dwProcessID);
    }

    ~WinProcessFinder() {
        if (m_hSnapShot != INVALID_HANDLE_VALUE) {
            CloseHandle(m_hSnapShot);
            m_hSnapShot = INVALID_HANDLE_VALUE;
        }
    }

    bool createSnapShot(DWORD dwFlag, DWORD dwProcessID) {
        if (m_hSnapShot != INVALID_HANDLE_VALUE)
            CloseHandle(m_hSnapShot);

        if (dwFlag == 0)
            m_hSnapShot = INVALID_HANDLE_VALUE;
        else
            m_hSnapShot = CreateToolhelp32Snapshot(dwFlag, dwProcessID);

        return (m_hSnapShot != INVALID_HANDLE_VALUE);
    }

    bool processFirst(PPROCESSENTRY32 ppe) const {
        BOOL fOk = Process32First(m_hSnapShot, ppe);

        if (fOk && (ppe->th32ProcessID == 0))
            fOk = processNext(ppe);  // remove the "[System Process]" (PID = 0)

        return fOk;
    }

    bool processNext(PPROCESSENTRY32 ppe) const {
        BOOL fOk = Process32Next(m_hSnapShot, ppe);

        if (fOk && (ppe->th32ProcessID == 0))
            fOk = processNext(ppe);  // remove the "[System Process]" (PID = 0)

        return fOk;
    }

    // Don't forgot pe.dwSize = sizeof(PROCESSENTRY32);
    bool processFind(DWORD dwProcessId, PPROCESSENTRY32 ppe) const {
        BOOL fFound = FALSE;

        for (BOOL fOk = processFirst(ppe); fOk; fOk = processNext(ppe)) {
            fFound = (ppe->th32ProcessID == dwProcessId);

            if (fFound)
                break;
        }

        return fFound;
    }

    bool processFind(PCTSTR pszExeName, PPROCESSENTRY32 ppe, BOOL bExceptSelf = false) const {
        BOOL fFound = FALSE;
        const DWORD dwCurrentPID = GetCurrentProcessId();

        for (BOOL fOk = processFirst(ppe); fOk; fOk = processNext(ppe)) {
            fFound = lstrcmpi(ppe->szExeFile, pszExeName) == 0;

            if (fFound) {
                if (bExceptSelf) {
                    if (ppe->th32ProcessID == dwCurrentPID) {
                        fFound = FALSE;
                        continue;
                    }
                }
                break;
            }
        }

        return fFound;
    }

    bool moduleFirst(PMODULEENTRY32 pme) const {
        return (Module32First(m_hSnapShot, pme));
    }

    bool moduleNext(PMODULEENTRY32 pme) const {
        return (Module32Next(m_hSnapShot, pme));
    }

    bool moduleFind(PVOID pvBaseAddr, PMODULEENTRY32 pme) const {
        BOOL fFound = FALSE;

        for (BOOL fOk = moduleFirst(pme); fOk; fOk = moduleNext(pme)) {
            fFound = (pme->modBaseAddr == pvBaseAddr);

            if (fFound)
                break;
        }

        return fFound;
    }

    bool moduleFind(PTSTR pszModName, PMODULEENTRY32 pme) const {
        BOOL fFound = FALSE;

        for (BOOL fOk = moduleFirst(pme); fOk; fOk = moduleNext(pme)) {
            fFound =
                (lstrcmpi(pme->szModule, pszModName) == 0) || (lstrcmpi(pme->szExePath, pszModName) == 0);

            if (fFound)
                break;
        }

        return fFound;
    }

    static bool IsExist(const std::wstring& processName) {
        WinProcessFinder wpf(TH32CS_SNAPPROCESS, 0);

        PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
        const BOOL b = wpf.processFind(UnicodeToTCHAR(processName).c_str(), &pe32);
        return !!b;
    }

    static bool IsExist(const std::string& processName) {
        WinProcessFinder wpf(TH32CS_SNAPPROCESS, 0);

        PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
        const BOOL b = wpf.processFind(AnsiToTCHAR(processName).c_str(), &pe32);
        return !!b;
    }

   private:
    HANDLE m_hSnapShot;
};
#endif
}  // namespace jhc

#endif
