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

#ifndef JHC_WIN_REGISTRY_HPP_
#define JHC_WIN_REGISTRY_HPP_

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
#endif
#include <vector>
#include <string>
#include "jhc/macros.hpp"

namespace jhc {
class RegKey {
   public:
    JHC_DISALLOW_COPY_MOVE(RegKey);

    // hkeyRoot can be one of :
    // HKEY_CLASSES_ROOT
    // HKEY_CURRENT_CONFIG
    // HKEY_CURRENT_USER
    // HKEY_LOCAL_MACHINE
    // HKEY_USERS
    //
    RegKey(HKEY hkeyRoot, const std::wstring& subKey);

    ~RegKey();

    // samDesired:
    // https://docs.microsoft.com/zh-cn/windows/desktop/SysInfo/registry-key-security-and-access-rights
    // samDesired can be one of:
    // KEY_ALL_ACCESS,
    // KEY_QUERY_VALUE,
    // KEY_READ,
    // KEY_WRITE,
    // KEY_WOW64_32KEY,
    // KEY_WOW64_64KEY
    // and so on.
    //
    LSTATUS open(REGSAM samDesired, bool bCreate);

    bool isOpen(void) const;

    HKEY getHandle(void) const;

    void attach(HKEY hkey);

    void detach();

    void close();

    HRESULT watchForChange(DWORD dwChangeFilter, bool bWatchSubtree);

    HRESULT waitForChange(DWORD dwChangeFilter, bool bWatchSubtree);

    static bool DeleteKey(HKEY hKey, LPCWSTR pszSubKey, LPCWSTR pszValName, bool bPrefer64View);

    bool deleteSubKeys(HKEY hKeyRoot, LPCTSTR lpSubKey, bool bPrefer64View);

    HRESULT getDWORDValue(LPCWSTR pszValueName, OUT DWORD& pdwDataOut) const;

    HRESULT getBINARYValue(LPCWSTR pszValueName, LPBYTE pbDataOut, int cbDataOut) const;

    HRESULT getSZValue(LPCWSTR pszValueName, OUT std::wstring& strValue) const;

    HRESULT getExpandSZValue(LPCWSTR pszValueName,
                             bool bRetrieveExpandedString,
                             OUT std::wstring& strValue) const;

    HRESULT getMultiSZValue(LPCWSTR pszValueName, OUT std::vector<std::wstring>& vStrValues) const;

    DWORD getValueBufferSize(LPCWSTR pszValueName) const;

    HRESULT setDWORDValue(LPCWSTR pszValueName, DWORD dwData);

    HRESULT setBINARYValue(LPCWSTR pszValueName, const LPBYTE pbData, int cbData);
    HRESULT setSZValue(LPCWSTR pszValueName, const std::wstring& strData);

    HRESULT setExpandSZValue(LPCWSTR pszValueName, const std::wstring& strData);

    HRESULT setMultiSZValue(LPCWSTR pszValueName, const std::vector<std::wstring>& vStrValues);

    HRESULT getSubKeys(std::vector<std::wstring>& subKeys);

   protected:
    void OnChange(HKEY hkey);

   private:
    HRESULT getValue(LPCWSTR pszValueName, DWORD dwTypeExpected, LPBYTE pbData, DWORD cbData) const;
    HRESULT setValue(LPCWSTR pszValueName, DWORD dwValueType, const LPBYTE pbData, int cbData);
    LPWSTR createDoubleNulTermList(const std::vector<std::wstring>& vStrValues) const;

    static unsigned int _stdcall NotifyWaitThreadProc(LPVOID pvParam);
    static bool regDeleteKey32_64(HKEY hKey, LPCWSTR pszSubKey, bool bPrefer64View);
    static bool regDeleteSubKeys(HKEY hKey, bool bPrefer64View);
    static BOOL regDelSubKeysRecurse(HKEY hKeyRoot, LPTSTR lpSubKey, bool bPrefer64View);

   private:
    HKEY m_hkeyRoot;
    mutable HKEY m_hkey;
    HANDLE m_hChangeEvent;
    HANDLE m_hNotifyThr;
    DWORD m_dwSamDesired;
    DWORD m_dwChangeFilter;
    std::wstring m_strSubKey;
    bool m_bWatchSubtree;
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/win_registry.cc"
#endif

#endif  // !JHC_WIN
#endif  // !JHC_WIN_REGISTRY_HPP_
