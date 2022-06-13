#include "jhc/config.hpp"
#ifdef JHC_NOT_HEADER_ONLY
#include "../win_process_finder.hpp"
#endif
#include "jhc/string_encode.hpp"

namespace jhc {
JHC_INLINE WinProcessFinder::WinProcessFinder(DWORD dwFlags, DWORD dwProcessID) {
    m_hSnapShot = INVALID_HANDLE_VALUE;
    createSnapShot(dwFlags, dwProcessID);
}

JHC_INLINE WinProcessFinder::~WinProcessFinder() {
    if (m_hSnapShot != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hSnapShot);
        m_hSnapShot = INVALID_HANDLE_VALUE;
    }
}

JHC_INLINE bool WinProcessFinder::createSnapShot(DWORD dwFlag, DWORD dwProcessID) {
    if (m_hSnapShot != INVALID_HANDLE_VALUE)
        CloseHandle(m_hSnapShot);

    if (dwFlag == 0)
        m_hSnapShot = INVALID_HANDLE_VALUE;
    else
        m_hSnapShot = CreateToolhelp32Snapshot(dwFlag, dwProcessID);

    return (m_hSnapShot != INVALID_HANDLE_VALUE);
}

JHC_INLINE bool WinProcessFinder::processFirst(PPROCESSENTRY32 ppe) const {
    BOOL fOk = Process32First(m_hSnapShot, ppe);

    if (fOk && (ppe->th32ProcessID == 0))
        fOk = processNext(ppe);  // remove the "[System Process]" (PID = 0)

    return fOk;
}

JHC_INLINE bool WinProcessFinder::processNext(PPROCESSENTRY32 ppe) const {
    BOOL fOk = Process32Next(m_hSnapShot, ppe);

    if (fOk && (ppe->th32ProcessID == 0))
        fOk = processNext(ppe);  // remove the "[System Process]" (PID = 0)

    return fOk;
}

// Don't forgot pe.dwSize = sizeof(PROCESSENTRY32);
JHC_INLINE bool WinProcessFinder::processFind(DWORD dwProcessId, PPROCESSENTRY32 ppe) const {
    BOOL fFound = FALSE;

    for (BOOL fOk = processFirst(ppe); fOk; fOk = processNext(ppe)) {
        fFound = (ppe->th32ProcessID == dwProcessId);

        if (fFound)
            break;
    }

    return fFound;
}

JHC_INLINE bool WinProcessFinder::processFind(PCTSTR pszExeName, PPROCESSENTRY32 ppe, BOOL bExceptSelf) const {
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

JHC_INLINE bool WinProcessFinder::moduleFirst(PMODULEENTRY32 pme) const {
    return (Module32First(m_hSnapShot, pme));
}

JHC_INLINE bool WinProcessFinder::moduleNext(PMODULEENTRY32 pme) const {
    return (Module32Next(m_hSnapShot, pme));
}

JHC_INLINE bool WinProcessFinder::moduleFind(PVOID pvBaseAddr, PMODULEENTRY32 pme) const {
    BOOL fFound = FALSE;

    for (BOOL fOk = moduleFirst(pme); fOk; fOk = moduleNext(pme)) {
        fFound = (pme->modBaseAddr == pvBaseAddr);

        if (fFound)
            break;
    }

    return fFound;
}

JHC_INLINE bool WinProcessFinder::moduleFind(PTSTR pszModName, PMODULEENTRY32 pme) const {
    BOOL fFound = FALSE;

    for (BOOL fOk = moduleFirst(pme); fOk; fOk = moduleNext(pme)) {
        fFound =
            (lstrcmpi(pme->szModule, pszModName) == 0) || (lstrcmpi(pme->szExePath, pszModName) == 0);

        if (fFound)
            break;
    }

    return fFound;
}

JHC_INLINE bool WinProcessFinder::IsExist(const std::wstring& processName) {
    WinProcessFinder wpf(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    const BOOL b = wpf.processFind(UnicodeToTCHAR(processName).c_str(), &pe32);
    return !!b;
}

JHC_INLINE bool WinProcessFinder::IsExist(const std::string& processName) {
    WinProcessFinder wpf(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    const BOOL b = wpf.processFind(AnsiToTCHAR(processName).c_str(), &pe32);
    return !!b;
}
}  // namespace jhc