#include "jhc/config.hpp"

#ifdef JHC_NOT_HEADER_ONLY
#include "../process_util.hpp"
#endif

#ifdef JHC_WIN
#include <tchar.h>
#include <strsafe.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#elif defined(_GNU_SOURCE)
#include <errno.h>
#endif
#include "jhc/macros.hpp"
#include "jhc/string_encode.hpp"
#include "jhc/filesystem.hpp"

#pragma warning(disable : 4996)

namespace jhc {
#ifdef JHC_WIN
JHC_INLINE bool ProcessUtil::IsRunAsAdminPrivilege(HANDLE hProcess) {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = (Elevation.TokenIsElevated != 0);
        }
    }

    if (hToken) {
        CloseHandle(hToken);
    }

    return !!fRet;
}

JHC_INLINE bool ProcessUtil::IsRunAsAdminPrivilege(DWORD dwPid) {
    HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
    if (!hProcess)
        return false;
    const bool ret = IsRunAsAdminPrivilege(hProcess);
    CloseHandle(hProcess);
    return ret;
}

JHC_INLINE bool ProcessUtil::SetUIPIMsgFilter(HWND hWnd, unsigned int uMessageID, bool bAllow) {
    OSVERSIONINFO VersionTmp;
    VersionTmp.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&VersionTmp);
    BOOL res = FALSE;

    if (VersionTmp.dwMajorVersion >= 6) {  // vista above.
        BOOL(WINAPI * pfnChangeMessageFilterEx)
        (HWND, UINT, DWORD, PCHANGEFILTERSTRUCT);
        BOOL(WINAPI * pfnChangeMessageFilter)
        (UINT, DWORD);

        CHANGEFILTERSTRUCT filterStatus;
        filterStatus.cbSize = sizeof(CHANGEFILTERSTRUCT);

        HINSTANCE hlib = LoadLibrary(_T("user32.dll"));
        if (hlib) {
            (FARPROC&)pfnChangeMessageFilterEx = GetProcAddress(hlib, "ChangeWindowMessageFilterEx");

            if (pfnChangeMessageFilterEx != NULL && hWnd != NULL) {
                res = pfnChangeMessageFilterEx(hWnd, uMessageID, (bAllow ? MSGFLT_ADD : MSGFLT_REMOVE),
                                               &filterStatus);
            }

            // If failed, try again.
            if (!res) {
                (FARPROC&)pfnChangeMessageFilter = GetProcAddress(hlib, "ChangeWindowMessageFilter");

                if (pfnChangeMessageFilter != NULL) {
                    res = pfnChangeMessageFilter(uMessageID, (bAllow ? MSGFLT_ADD : MSGFLT_REMOVE));
                }
            }

            FreeLibrary(hlib);
        }
    }
    else {
        res = TRUE;
    }

    return !!res;
}

JHC_INLINE bool ProcessUtil::CreateNewProcess(const std::wstring& path, const std::wstring& param, DWORD* dwPID, HANDLE* pProcess) {
    WCHAR szDir[MAX_PATH] = {0};
    StringCchPrintfW(szDir, MAX_PATH, L"%s", path.c_str());
    PathRemoveFileSpecW(szDir);

    WCHAR szFullCMD[1024];
    if (param.length() > 0)
        StringCchPrintfW(szFullCMD, 1024, L"\"%s\" %s", path.c_str(), param.c_str());
    else
        StringCchPrintfW(szFullCMD, 1024, L"\"%s\"", path.c_str());

    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi = {0};
    if (!CreateProcessW(NULL, szFullCMD, NULL, NULL, FALSE, 0, NULL, szDir, &si, &pi))
        return false;

    if (pi.hThread)
        CloseHandle(pi.hThread);

    if (pProcess) {
        *pProcess = pi.hProcess;
    }
    else {
        SAFE_CLOSE(pi.hProcess);
    }

    if (dwPID) {
        *dwPID = pi.dwProcessId;
    }

    return true;
}

#endif

// On windows, path is encoded by ANSI, otherwise, is UTF8.
//
JHC_INLINE std::string ProcessUtil::GetCurrentExePath() {
    std::string ret;
#ifdef JHC_WIN
    char* pBuf = NULL;
    DWORD dwBufSize = MAX_PATH;

    do {
        pBuf = (char*)malloc(dwBufSize);
        if (!pBuf)
            break;
        memset(pBuf, 0, dwBufSize);

        DWORD dwGot = GetModuleFileNameA(NULL, pBuf, dwBufSize);
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            free(pBuf);
            dwBufSize *= 2;
        }
        else {
            break;
        }
    } while (true);

    if (pBuf) {
        ret = pBuf;
        free(pBuf);
    }
#elif defined(JHC_MACOS)
    return getprogname();
#elif defined(_GNU_SOURCE)
    return program_invocation_name;
#endif
    return ret;
}

// On windows, directory is encoded by ANSI, otherwise, is UTF8.
//
JHC_INLINE std::string ProcessUtil::GetCurrentExeDirectory() {
    const std::string path = GetCurrentExePath();
#ifdef JHC_WIN
    const std::wstring pathW = StringEncode::AnsiToUnicode(path);
    fs::path p(pathW);
    p.remove_filename();
    return StringEncode::UnicodeToAnsi(p.wstring());
#else
    fs::path p(path);
    p.remove_filename();
    return p.u8string();
#endif
}

#ifdef JHC_WIN
JHC_INLINE BOOL CALLBACK ProcessUtil::EnumResourceNameCallback(HMODULE hModule, LPCWSTR lpType, LPWSTR lpName, LONG_PTR lParam) {
    std::list<std::string>* pList = (std::list<std::string>*)lParam;

    HRSRC hResInfo = FindResourceW(hModule, lpName, lpType);
    DWORD cbResource = SizeofResource(hModule, hResInfo);

    HGLOBAL hResData = LoadResource(hModule, hResInfo);
    if (hResData) {
        const BYTE* pResource = (const BYTE*)LockResource(hResData);

        if (pResource) {
            std::string strU8;
            strU8.assign((const char*)pResource, cbResource);
            pList->push_back(strU8);
        }

        UnlockResource(hResData);
        FreeResource(hResData);
    }

    return TRUE;  // Keep going
}

JHC_INLINE bool ProcessUtil::GetExeOrDllManifest(const std::wstring& path, std::list<std::string>& manifests) {
    HMODULE hModule = LoadLibraryExW(path.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (!hModule)
        return false;

    EnumResourceNames(hModule, RT_MANIFEST, EnumResourceNameCallback, (LONG_PTR)&manifests);
    FreeLibrary(hModule);

    return true;
}
#endif
}  // namespace jhc