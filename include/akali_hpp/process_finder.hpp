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

#ifndef AKALI_PROCESS_FINDER_HPP__
#define AKALI_PROCESS_FINDER_HPP__

#include "akali_hpp/arch.hpp"

#ifdef AKALI_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif
#include <TlHelp32.h>
#endif

#include <string>
#include "akali_hpp/string_encode.hpp"
#include "akali_hpp/macros.hpp"

namespace akali_hpp {
#ifdef AKALI_WIN
class WinProcessFinder {
   public:
    AKALI_DISALLOW_COPY_AND_ASSIGN(WinProcessFinder);

    WinProcessFinder(DWORD dwFlags = TH32CS_SNAPALL, DWORD dwProcessID = 0) {
        m_hSnapShot = INVALID_HANDLE_VALUE;
        CreateSnapShot(dwFlags, dwProcessID);
    }

    ~WinProcessFinder() {
        if (m_hSnapShot != INVALID_HANDLE_VALUE) {
            CloseHandle(m_hSnapShot);
            m_hSnapShot = INVALID_HANDLE_VALUE;
        }
    }

    BOOL CreateSnapShot(DWORD dwFlag, DWORD dwProcessID) {
        if (m_hSnapShot != INVALID_HANDLE_VALUE)
            CloseHandle(m_hSnapShot);

        if (dwFlag == 0)
            m_hSnapShot = INVALID_HANDLE_VALUE;
        else
            m_hSnapShot = CreateToolhelp32Snapshot(dwFlag, dwProcessID);

        return (m_hSnapShot != INVALID_HANDLE_VALUE);
    }
    BOOL ProcessFirst(PPROCESSENTRY32 ppe) const {
        BOOL fOk = Process32First(m_hSnapShot, ppe);

        if (fOk && (ppe->th32ProcessID == 0))
            fOk = ProcessNext(ppe);  // remove the "[System Process]" (PID = 0)

        return fOk;
    }

    BOOL ProcessNext(PPROCESSENTRY32 ppe) const {
        BOOL fOk = Process32Next(m_hSnapShot, ppe);

        if (fOk && (ppe->th32ProcessID == 0))
            fOk = ProcessNext(ppe);  // remove the "[System Process]" (PID = 0)

        return fOk;
    }

    // Don't forgot pe.dwSize = sizeof(PROCESSENTRY32);
    BOOL ProcessFind(DWORD dwProcessId, PPROCESSENTRY32 ppe) const {
        BOOL fFound = FALSE;

        for (BOOL fOk = ProcessFirst(ppe); fOk; fOk = ProcessNext(ppe)) {
            fFound = (ppe->th32ProcessID == dwProcessId);

            if (fFound)
                break;
        }

        return fFound;
    }

    BOOL ProcessFind(PCTSTR pszExeName, PPROCESSENTRY32 ppe, BOOL bExceptSelf = false) const {
        BOOL fFound = FALSE;
        DWORD dwCurrentPID = GetCurrentProcessId();

        for (BOOL fOk = ProcessFirst(ppe); fOk; fOk = ProcessNext(ppe)) {
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

    BOOL ModuleFirst(PMODULEENTRY32 pme) const {
        return (Module32First(m_hSnapShot, pme));
    }

    BOOL ModuleNext(PMODULEENTRY32 pme) const {
        return (Module32Next(m_hSnapShot, pme));
    }

    BOOL ModuleFind(PVOID pvBaseAddr, PMODULEENTRY32 pme) const {
        BOOL fFound = FALSE;

        for (BOOL fOk = ModuleFirst(pme); fOk; fOk = ModuleNext(pme)) {
            fFound = (pme->modBaseAddr == pvBaseAddr);

            if (fFound)
                break;
        }

        return fFound;
    }

    BOOL ModuleFind(PTSTR pszModName, PMODULEENTRY32 pme) const {
        BOOL fFound = FALSE;

        for (BOOL fOk = ModuleFirst(pme); fOk; fOk = ModuleNext(pme)) {
            fFound =
                (lstrcmpi(pme->szModule, pszModName) == 0) || (lstrcmpi(pme->szExePath, pszModName) == 0);

            if (fFound)
                break;
        }

        return fFound;
    }

    static bool IsProcessExist(const std::wstring& processName) {
        WinProcessFinder wpf(TH32CS_SNAPPROCESS, 0);

        PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
        BOOL b = wpf.ProcessFind(UnicodeToTCHAR(processName).c_str(), &pe32);
        return !!b;
    }

   private:
    HANDLE m_hSnapShot;
};
#endif
}  // namespace akali_hpp

#endif