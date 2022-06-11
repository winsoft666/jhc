#ifdef JHC_NOT_HEADER_ONLY
#include "../path_util.hpp"
#endif

#ifdef JHC_WIN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <shlobj_core.h>
#endif
#include "jhc/os_ver.hpp"

namespace jhc {
#ifdef JHC_WIN
std::wstring PathUtil::GetWindowsFolder() {
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

std::wstring PathUtil::GetSystemFolder() {
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

std::wstring PathUtil::GetTempFolder() {
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

std::wstring PathUtil::GetLocalAppDataFolder() {
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

bool PathUtil::OpenWinExplorerAndLocate(const std::wstring& path) {
    bool result = false;
    ITEMIDLIST* pidl = ILCreateFromPathW(path.c_str());
    if (pidl) {
        result = (S_OK == SHOpenFolderAndSelectItems(pidl, 0, NULL, 0));
        ILFree(pidl);
    }
    return result;
}

std::wstring PathUtil::GetWinExplorerDisplayName(const std::wstring& path) {
    SHFILEINFOW sfi;
    ZeroMemory(&sfi, sizeof(SHFILEINFOW));
    DWORD_PTR dwRet = ::SHGetFileInfoW(path.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFOW), SHGFI_DISPLAYNAME);
    if (dwRet != 0) {
        return std::wstring(sfi.szDisplayName);
    }
    return std::wstring();
}
#endif
}  // namespace jhc