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

#ifndef JHC_WIN_INI_HPP_
#define JHC_WIN_INI_HPP_

#include "jhc/arch.hpp"
#ifdef JHC_WIN
#include <tchar.h>
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif  // !_INC_WINDOWS
#include <string>
#include <vector>
#include <strsafe.h>
#include <assert.h>

namespace jhc {
class Ini {
   public:
    Ini() = default;

    Ini(const std::wstring& file_path) :
        ini_file_path_(file_path) {}

    Ini(std::wstring&& file_path) :
        ini_file_path_(std::move(file_path)) {}

    ~Ini() = default;

    void setIniFilePath(const std::wstring& file_path) noexcept { ini_file_path_ = file_path; }

    std::wstring iniFilePath() const noexcept { return ini_file_path_; }

    bool readInt(const std::wstring& item, const std::wstring& sub_item, UINT* result) noexcept {
        if (ini_file_path_.length() == 0)
            return false;
        INT iDefault = 0;
        SetLastError(0);
        UINT ret =
            GetPrivateProfileIntW(item.c_str(), sub_item.c_str(), iDefault, ini_file_path_.c_str());
        DWORD dwGLE = GetLastError();
        if (dwGLE == 0) {
            if (result)
                *result = ret;
            return true;
        }
        return false;
    }

    UINT readInt(const std::wstring& item,
                 const std::wstring& sub_item,
                 UINT default_value) noexcept {
        if (ini_file_path_.length() == 0)
            return default_value;

        SetLastError(0);
        return GetPrivateProfileIntW(item.c_str(), sub_item.c_str(), default_value,
                                     ini_file_path_.c_str());
    }

    std::wstring readString(const std::wstring& item,
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

    bool readString(const std::wstring& item,
                    const std::wstring& sub_item,
                    std::wstring* result) noexcept {
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
            if (result)
                *result = pBuf;
        }
        free(pBuf);

        return ret;
    }

    bool writeInt(const std::wstring& item, const std::wstring& sub_item, LONG value) noexcept {
        if (ini_file_path_.length() == 0)
            return false;

        WCHAR szValue[50];
        StringCchPrintfW(szValue, 50, L"%ld", value);
        return writeString(item, sub_item, szValue);
    }

    bool writeColor(const std::wstring& item, const std::wstring& sub_item, COLORREF value) noexcept {
        WCHAR szValue[60];
        StringCchPrintfW(szValue, 60, L"%d,%d,%d,%d", (BYTE)(value >> 24), (BYTE)(value >> 16),
                         (BYTE)(value >> 8), (BYTE)value);

        return writeString(item, sub_item, szValue);
    }

    bool writePoint(const std::wstring& item, const std::wstring& sub_item, POINT value) noexcept {
        WCHAR szValue[50];
        StringCchPrintfW(szValue, 50, L"%d,%d", value.x, value.y);

        return writeString(item, sub_item, szValue);
    }

    bool writeSize(const std::wstring& item, const std::wstring& sub_item, SIZE value) noexcept {
        WCHAR szValue[50];
        StringCchPrintfW(szValue, 50, L"%d,%d", value.cx, value.cy);

        return writeString(item, sub_item, szValue);
    }

    bool writeRect(const std::wstring& item, const std::wstring& sub_item, RECT value) noexcept {
        WCHAR szValue[50];
        StringCchPrintfW(szValue, 50, L"%d,%d,%d,%d", value.left, value.top, value.right, value.bottom);

        return writeString(item, sub_item, szValue);
    }

    bool writeString(const std::wstring& item,
                     const std::wstring& sub_item,
                     const std::wstring& value) noexcept {
        if (ini_file_path_.length() == 0)
            return false;

        return !!WritePrivateProfileStringW(item.c_str(), sub_item.c_str(), value.c_str(),
                                            ini_file_path_.c_str());
    }

    bool readRect(const std::wstring& item, const std::wstring& sub_item, RECT* result) noexcept {
        if (ini_file_path_.length() == 0)
            return false;

        WCHAR szReadData[64] = L"";
        ZeroMemory(result, sizeof(result));

        GetPrivateProfileStringW(item.c_str(), sub_item.c_str(), L"", szReadData, 64,
                                 ini_file_path_.c_str());

        if (szReadData[0] != 0) {
            LPCWSTR pszString = szReadData;
            if (result) {
                result->left = SwitchStringToValue(pszString);
                result->top = SwitchStringToValue(pszString);
                result->right = SwitchStringToValue(pszString);
                result->bottom = SwitchStringToValue(pszString);
            }
            return true;
        }

        return false;
    }

    bool readSize(const std::wstring& item, const std::wstring& sub_item, SIZE* result) noexcept {
        if (ini_file_path_.length() == 0)
            return false;
        WCHAR szReadData[64] = L"";
        ZeroMemory(result, sizeof(result));

        GetPrivateProfileStringW(item.c_str(), sub_item.c_str(), L"", szReadData, 64,
                                 ini_file_path_.c_str());

        if (szReadData[0] != 0) {
            LPCWSTR pszString = szReadData;
            if (result) {
                result->cx = SwitchStringToValue(pszString);
                result->cy = SwitchStringToValue(pszString);
            }
            return true;
        }

        return false;
    }

    bool readPoint(const std::wstring& item, const std::wstring& sub_item, POINT* result) noexcept {
        if (ini_file_path_.length() == 0)
            return false;
        WCHAR szReadData[64] = L"";
        ZeroMemory(result, sizeof(result));

        GetPrivateProfileStringW(item.c_str(), sub_item.c_str(), L"", szReadData, 64,
                                 ini_file_path_.c_str());

        if (szReadData[0] != 0) {
            LPCWSTR pszString = szReadData;
            if (result) {
                result->x = SwitchStringToValue(pszString);
                result->y = SwitchStringToValue(pszString);
            }

            return true;
        }

        return false;
    }

    bool readColor(const std::wstring& item,
                   const std::wstring& sub_item,
                   COLORREF* result) noexcept {
        if (ini_file_path_.length() == 0)
            return false;
        WCHAR szReadData[64] = L"";
        ZeroMemory(result, sizeof(result));
        GetPrivateProfileStringW(item.c_str(), sub_item.c_str(), L"", szReadData, 64,
                                 ini_file_path_.c_str());

        if (szReadData[0] != 0) {
            LPCWSTR pszString = szReadData;
            if (result)
                *result = SwitchStringToValue(pszString) << 24 | SwitchStringToValue(pszString) << 16 |
                          SwitchStringToValue(pszString) << 8 | SwitchStringToValue(pszString);

            return true;
        }

        return false;
    }

   protected:
    LONG SwitchStringToValue(LPCWSTR& pszSring) noexcept {
        assert((pszSring != NULL) && (pszSring[0] != 0));
        if ((pszSring == NULL) || (pszSring[0] == 0))
            return 0L;

        while (((pszSring[0] > 0) && (pszSring[0] < L'0')) || (pszSring[0] > L'9'))
            pszSring++;

        LONG lValue = 0L;
        while ((pszSring[0] >= L'0') && (pszSring[0] <= L'9')) {
            lValue = lValue * 10L + pszSring[0] - L'0';
            ++pszSring;
        }

        return lValue;
    }

    std::wstring ini_file_path_;
};
}  // namespace jhc
#endif  // !JHC_WIN
#endif  // !JHC_WIN_INI_HPP_
