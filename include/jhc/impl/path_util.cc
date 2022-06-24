#include "jhc/config.hpp"

#ifdef JHC_NOT_HEADER_ONLY
#include "../path_util.hpp"
#endif

#ifdef JHC_WIN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <shlobj_core.h>
#include <shellapi.h>
#endif
#include "jhc/os_ver.hpp"
#include "jhc/string_helper.hpp"

namespace jhc {
#ifdef JHC_WIN
JHC_INLINE std::wstring PathUtil::GetWindowsFolder() {
    wchar_t szBuf[MAX_PATH] = {0};
    const DWORD result = ::GetWindowsDirectoryW(szBuf, MAX_PATH);
    if (result == 0)
        return std::wstring();

    std::wstring tempPath = szBuf;
    if (!tempPath.empty())
        if (tempPath.back() != L'\\')
            tempPath.push_back(L'\\');
    return tempPath;
}

JHC_INLINE std::wstring PathUtil::GetSystemFolder() {
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

JHC_INLINE std::wstring PathUtil::GetTempFolder() {
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

JHC_INLINE std::wstring PathUtil::GetLocalAppDataFolder() {
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
        if (S_OK == ::SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, buffer))
            tempPath = buffer;
    }

    if (!tempPath.empty())
        if (tempPath.back() != L'\\')
            tempPath.push_back(L'\\');

    return tempPath;
}

JHC_INLINE bool PathUtil::OpenWinExplorerAndLocate(const std::wstring& path) {
    bool result = false;
    ITEMIDLIST* pidl = ILCreateFromPathW(path.c_str());
    if (pidl) {
        result = (S_OK == SHOpenFolderAndSelectItems(pidl, 0, NULL, 0));
        ILFree(pidl);
    }
    return result;
}

JHC_INLINE std::wstring PathUtil::GetWinExplorerDisplayName(const std::wstring& path) {
    SHFILEINFOW sfi;
    ZeroMemory(&sfi, sizeof(SHFILEINFOW));
    DWORD_PTR dwRet = ::SHGetFileInfoW(path.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFOW), SHGFI_DISPLAYNAME);
    if (dwRet != 0) {
        return std::wstring(sfi.szDisplayName);
    }
    return std::wstring();
}

JHC_INLINE std::string PathUtil::ReplaceKnownEnvToWow6432(const std::string& src) {
    if (!StringHelper::IsContains(src, "%"))
        return src;

    BOOL isWow64 = FALSE;
#ifdef JHC_WIN32
    HANDLE hProcess = GetCurrentProcess();
    IsWow64Process(hProcess, &isWow64);
#endif

    if (!isWow64)
        return src;

    std::string sl = src;
    sl = StringHelper::Replace(sl, "%ProgramFiles%", "%ProgramW6432%", 0, true);
    sl = StringHelper::Replace(sl, "%CommonProgramFiles%", "%CommonProgramW6432%", 0, true);

    return sl;
}

JHC_INLINE std::wstring PathUtil::ReplaceKnownEnvToWow6432(const std::wstring& src) {
    if (!StringHelper::IsContains(src, L"%"))
        return src;

    BOOL isWow64 = FALSE;
#ifdef JHC_WIN32
    HANDLE hProcess = GetCurrentProcess();
    IsWow64Process(hProcess, &isWow64);
#endif

    if (!isWow64)
        return src;

    std::wstring sl = src;
    sl = StringHelper::Replace(sl, L"%ProgramFiles%", L"%ProgramW6432%", 0, true);
    sl = StringHelper::Replace(sl, L"%CommonProgramFiles%", L"%CommonProgramW6432%", 0, true);

    return sl;
}

JHC_INLINE std::string PathUtil::ExpandEnvString(const std::string& src, bool disableWow64FsRedirection) {
    std::string srcCopy;
    PVOID OldValue = NULL;
    if (disableWow64FsRedirection) {
        Wow64DisableWow64FsRedirection(&OldValue);
        srcCopy = ReplaceKnownEnvToWow6432(src);
    }
    else {
        srcCopy = src;
    }

    DWORD dwCount = ExpandEnvironmentStringsA(srcCopy.c_str(), NULL, 0);
    std::vector<char> buf(dwCount + 1);
    ExpandEnvironmentStringsA(srcCopy.c_str(), &buf[0], dwCount);

    if (disableWow64FsRedirection)
        Wow64RevertWow64FsRedirection(OldValue);

    return std::string(&buf[0]);
}

JHC_INLINE std::wstring PathUtil::ExpandEnvString(const std::wstring& src, bool disableWow64FsRedirection) {
    std::wstring srcCopy;
    PVOID OldValue = NULL;
    if (disableWow64FsRedirection) {
        Wow64DisableWow64FsRedirection(&OldValue);
        srcCopy = ReplaceKnownEnvToWow6432(src);
    }
    else {
        srcCopy = src;
    }

    DWORD dwCount = ExpandEnvironmentStringsW(srcCopy.c_str(), NULL, 0);
    std::vector<wchar_t> buf(dwCount + 1);
    ExpandEnvironmentStringsW(srcCopy.c_str(), &buf[0], dwCount);

    if (disableWow64FsRedirection)
        Wow64RevertWow64FsRedirection(OldValue);

    return std::wstring(&buf[0]);
}
#endif
}  // namespace jhc