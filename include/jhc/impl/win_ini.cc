#include "jhc/config.hpp"
#ifdef JHC_NOT_HEADER_ONLY
#include "../win_ini.hpp"
#endif

#ifdef JHC_WIN
#include <tchar.h>
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif  // !_INC_WINDOWS
#include <strsafe.h>
#include <assert.h>
#include <vector>

namespace jhc {
Ini::Ini(const std::wstring& file_path) :
    ini_file_path_(file_path) {}

Ini::Ini(std::wstring&& file_path) :
    ini_file_path_(std::move(file_path)) {}

void Ini::setIniFilePath(const std::wstring& file_path) noexcept {
    ini_file_path_ = file_path;
}

std::wstring Ini::iniFilePath() const noexcept {
    return ini_file_path_;
}

bool Ini::readInt(const std::wstring& item, const std::wstring& sub_item, unsigned int& result) noexcept {
    if (ini_file_path_.length() == 0)
        return false;
    INT iDefault = 0;
    SetLastError(0);
    UINT ret =
        GetPrivateProfileIntW(item.c_str(), sub_item.c_str(), iDefault, ini_file_path_.c_str());
    DWORD dwGLE = GetLastError();
    if (dwGLE == 0) {
        result = ret;
        return true;
    }
    return false;
}

UINT Ini::readInt(const std::wstring& item,
                  const std::wstring& sub_item,
                  UINT default_value) noexcept {
    if (ini_file_path_.length() == 0)
        return default_value;

    SetLastError(0);
    return GetPrivateProfileIntW(item.c_str(), sub_item.c_str(), default_value,
                                 ini_file_path_.c_str());
}

std::wstring Ini::readString(const std::wstring& item,
                             const std::wstring& sub_item,
                             const std::wstring& default_value) noexcept {
    if (ini_file_path_.length() == 0)
        return default_value;

    std::vector<wchar_t> buf(1024, 0);
    DWORD read = GetPrivateProfileStringW(item.c_str(), sub_item.c_str(), default_value.c_str(),
                                          buf.data(), 1024, ini_file_path_.c_str());

    std::wstring result;
    result.assign(buf.data(), read);

    return result;
}

bool Ini::readString(const std::wstring& item,
                     const std::wstring& sub_item,
                     std::wstring& result) noexcept {
    if (ini_file_path_.length() == 0)
        return false;

    bool ret = false;
    int iBufSize = 255;
    WCHAR* pBuf = NULL;
    do {
        pBuf = (WCHAR*)malloc(iBufSize * sizeof(WCHAR));
        if (!pBuf) {
            break;
        }
        memset(pBuf, 0, iBufSize * sizeof(WCHAR));
        SetLastError(0);
        DWORD dwRet = GetPrivateProfileStringW(item.c_str(), sub_item.c_str(), L"", pBuf, iBufSize,
                                               ini_file_path_.c_str());
        DWORD dwGLE = GetLastError();
        if (dwRet == 0) {
            ret = (dwGLE == 0);
            break;
        }
        else if (dwRet == iBufSize - 1) {
            free(pBuf);
            pBuf = NULL;
            iBufSize += 1024;
        }
        else {
            ret = true;
            break;
        }
    } while (true);

    if (ret) {
            result = pBuf;
    }
    free(pBuf);

    return ret;
}

bool Ini::writeInt(const std::wstring& item, const std::wstring& sub_item, unsigned int value) noexcept {
    if (ini_file_path_.length() == 0)
        return false;

    WCHAR szValue[50];
    StringCchPrintfW(szValue, 50, L"%ld", value);
    return writeString(item, sub_item, szValue);
}

bool Ini::writeString(const std::wstring& item,
                      const std::wstring& sub_item,
                      const std::wstring& value) noexcept {
    if (ini_file_path_.length() == 0)
        return false;

    return !!WritePrivateProfileStringW(item.c_str(), sub_item.c_str(), value.c_str(),
                                        ini_file_path_.c_str());
}
}  // namespace jhc
#endif