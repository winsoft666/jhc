#ifdef JHC_NOT_HEADER_ONLY
#include "../win_registry.hpp"
#endif

#ifdef JHC_WIN
#include <process.h>
#include <strsafe.h>
#include "jhc/os_ver.hpp"

namespace jhc {
RegKey::RegKey(HKEY hkeyRoot, const std::wstring& subKey) :
    m_hkeyRoot(hkeyRoot),
    m_hkey(NULL),
    m_hChangeEvent(NULL),
    m_hNotifyThr(NULL),
    m_bWatchSubtree(false),
    m_dwChangeFilter(0),
    m_dwSamDesired(0),
    m_strSubKey(subKey) {}

RegKey::~RegKey() {
    close();

    if (m_hChangeEvent)
        CloseHandle(m_hChangeEvent);
}

LSTATUS RegKey::open(REGSAM samDesired, bool bCreate) {
    LSTATUS dwResult = ERROR_SUCCESS;
    close();

    m_dwSamDesired = samDesired;
    if (bCreate) {
        DWORD dwDisposition;
        dwResult = RegCreateKeyExW(m_hkeyRoot, m_strSubKey.c_str(), 0, NULL, 0, samDesired, NULL,
                                   &m_hkey, &dwDisposition);
    }
    else {
        dwResult = RegOpenKeyExW(m_hkeyRoot, m_strSubKey.c_str(), 0, samDesired, &m_hkey);
    }

    return dwResult;
}

bool RegKey::isOpen(void) const {
    return NULL != m_hkey;
}

HKEY RegKey::getHandle(void) const {
    return m_hkey;
}

void RegKey::attach(HKEY hkey) {
    close();
    m_strSubKey.clear();

    m_hkeyRoot = NULL;
    m_hkey = hkey;
}

void RegKey::detach(void) {
    m_hkey = NULL;
}

void RegKey::close(void) {
    if (NULL != m_hkey) {
        HKEY hkeyTemp = m_hkey;
        m_hkey = NULL;
        RegCloseKey(hkeyTemp);
    }

    if (m_hNotifyThr) {
        WaitForSingleObject(m_hNotifyThr, INFINITE);
    }

    if (m_hNotifyThr) {
        CloseHandle(m_hNotifyThr);
        m_hNotifyThr = NULL;
    }

    m_dwSamDesired = 0;
}

HRESULT RegKey::watchForChange(DWORD dwChangeFilter, bool bWatchSubtree) {
    HRESULT hr = E_FAIL;

    if (NULL != m_hChangeEvent || NULL == m_hkey)
        return E_FAIL;

    m_hChangeEvent = CreateEventW(NULL, FALSE, FALSE, NULL);

    if (NULL == m_hChangeEvent) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    m_dwChangeFilter = dwChangeFilter;
    m_bWatchSubtree = bWatchSubtree;

    unsigned int uThreadId = 0;
    m_hNotifyThr = (HANDLE)_beginthreadex(NULL, 0, NotifyWaitThreadProc, this, 0, &uThreadId);

    if (m_hNotifyThr) {
        hr = NOERROR;
    }

    return hr;
}

HRESULT RegKey::waitForChange(DWORD dwChangeFilter, bool bWatchSubtree) {
    HRESULT hr = NOERROR;
    LONG lResult = RegNotifyChangeKeyValue(m_hkey, bWatchSubtree, dwChangeFilter, NULL, FALSE);

    if (ERROR_SUCCESS != lResult) {
        hr = HRESULT_FROM_WIN32(lResult);
    }

    return hr;
}

bool RegKey::DeleteKey(HKEY hKey, LPCWSTR pszSubKey, LPCWSTR pszValName, bool bPrefer64View) {
    HKEY hSubKey = NULL;

    if (pszSubKey) {
        REGSAM rsam = KEY_READ | KEY_WRITE;

        if (bPrefer64View)
            rsam |= KEY_WOW64_64KEY;

        RegOpenKeyExW(hKey, pszSubKey, 0, rsam, &hSubKey);
    }
    else {
        hSubKey = hKey;
    }

    if (hSubKey) {
        if (pszValName) {
            if (RegDeleteValueW(hSubKey, pszValName) == ERROR_SUCCESS) {
                if (hSubKey != hKey) {
                    if (hSubKey)
                        RegCloseKey(hSubKey);
                }

                return true;
            }
        }
        else {
            if (regDeleteSubKeys(hSubKey, bPrefer64View)) {
                if (hSubKey != hKey) {
                    if (hSubKey)
                        RegCloseKey(hSubKey);
                }

                return regDeleteKey32_64(hKey, pszSubKey, bPrefer64View);
            }
        }

        if (hSubKey != hKey) {
            if (hSubKey)
                RegCloseKey(hSubKey);
        }
    }

    return false;
}

bool RegKey::deleteSubKeys(HKEY hKeyRoot, LPCTSTR lpSubKey, bool bPrefer64View) {
    TCHAR szDelKey[MAX_PATH * 2];

    StringCchCopy(szDelKey, MAX_PATH * 2, lpSubKey);
    return regDelSubKeysRecurse(hKeyRoot, szDelKey, bPrefer64View) == TRUE;
}

HRESULT RegKey::getDWORDValue(LPCWSTR pszValueName, OUT DWORD& pdwDataOut) const {
    return getValue(pszValueName, REG_DWORD, (LPBYTE)(&pdwDataOut), sizeof(DWORD));
}

HRESULT RegKey::getBINARYValue(LPCWSTR pszValueName, LPBYTE pbDataOut, int cbDataOut) const {
    return getValue(pszValueName, REG_BINARY, pbDataOut, cbDataOut);
}

HRESULT RegKey::getSZValue(LPCWSTR pszValueName, OUT std::wstring& strValue) const {
    HRESULT hr = E_FAIL;
    DWORD cb = getValueBufferSize(pszValueName);

    if (cb == 0)
        return hr;

    WCHAR* pTemp = (WCHAR*)malloc(cb + sizeof(WCHAR));
    if (!pTemp)
        return E_OUTOFMEMORY;
    memset(pTemp, 0, cb + sizeof(WCHAR));

    hr = getValue(pszValueName, REG_SZ, (LPBYTE)pTemp, cb);
    if (hr == S_OK)
        strValue = pTemp;

    SAFE_FREE(pTemp);

    return hr;
}

HRESULT RegKey::getExpandSZValue(LPCWSTR pszValueName,
                                 bool bRetrieveExpandedString,
                                 OUT std::wstring& strValue) const {
    WCHAR szBuf[MAX_PATH] = {0};
    DWORD dwSize = MAX_PATH * sizeof(WCHAR);
    DWORD dwFlags = RRF_RT_ANY | RRF_ZEROONFAILURE;
    if (!bRetrieveExpandedString) {
        dwFlags |= RRF_NOEXPAND;
        dwFlags |= RRF_RT_REG_EXPAND_SZ;
    }
    else {
        dwFlags |= RRF_RT_REG_SZ;
    }

    if (m_dwSamDesired & KEY_WOW64_64KEY)
        dwFlags |= RRF_SUBKEY_WOW6464KEY;
    else if (m_dwSamDesired & KEY_WOW64_32KEY)
        dwFlags |= RRF_SUBKEY_WOW6432KEY;
    else
        dwFlags |= RRF_SUBKEY_WOW6432KEY;

    LSTATUS status =
        RegGetValueW(m_hkeyRoot, m_strSubKey.c_str(), pszValueName, dwFlags, NULL, szBuf, &dwSize);
    if (status == ERROR_MORE_DATA) {
        WCHAR* pBuf = new WCHAR[dwSize / sizeof(WCHAR)];
        memset(pBuf, 0, dwSize);
        status =
            RegGetValueW(m_hkeyRoot, m_strSubKey.c_str(), pszValueName, dwFlags, NULL, pBuf, &dwSize);

        strValue = pBuf;
        SAFE_DELETE_ARRAY(pBuf);
    }
    else if (status == ERROR_SUCCESS) {
        strValue = szBuf;
    }

    return status;
}

HRESULT RegKey::getMultiSZValue(LPCWSTR pszValueName, OUT std::vector<std::wstring>& vStrValues) const {
    HRESULT hr = E_FAIL;
    DWORD cb = getValueBufferSize(pszValueName);

    if (cb == 0)
        return hr;

    WCHAR* pTemp = (WCHAR*)malloc(cb + sizeof(WCHAR));
    if (!pTemp)
        return E_OUTOFMEMORY;

    memset(pTemp, 0, cb + sizeof(WCHAR));
    WCHAR* pBegin = pTemp;

    hr = getValue(pszValueName, REG_MULTI_SZ, (LPBYTE)pTemp, cb);
    if (SUCCEEDED(hr)) {
        while (pTemp && TEXT('\0') != *pTemp) {
            vStrValues.push_back(std::wstring(pTemp));
            pTemp += lstrlenW(pTemp) + 1;
        }
    }

    SAFE_FREE(pBegin);

    return hr;
}

DWORD RegKey::getValueBufferSize(LPCWSTR pszValueName) const {
    DWORD dwType;
    DWORD cbData = 0;
    DWORD dwResult = RegQueryValueExW(m_hkey, pszValueName, 0, &dwType, NULL, (LPDWORD)&cbData);
    return cbData;
}

HRESULT RegKey::setDWORDValue(LPCWSTR pszValueName, DWORD dwData) {
    return setValue(pszValueName, REG_DWORD, (const LPBYTE)&dwData, sizeof(dwData));
}

HRESULT RegKey::setBINARYValue(LPCWSTR pszValueName, const LPBYTE pbData, int cbData) {
    return setValue(pszValueName, REG_BINARY, pbData, cbData);
}

HRESULT RegKey::setSZValue(LPCWSTR pszValueName, const std::wstring& strData) {
    return setValue(pszValueName, REG_SZ, (const LPBYTE)strData.c_str(),
                    (strData.length()) * sizeof(WCHAR));
}

HRESULT RegKey::setExpandSZValue(LPCWSTR pszValueName, const std::wstring& strData) {
    return setValue(pszValueName, REG_EXPAND_SZ, (const LPBYTE)strData.c_str(),
                    (strData.length()) * sizeof(WCHAR));
}

HRESULT RegKey::setMultiSZValue(LPCWSTR pszValueName, const std::vector<std::wstring>& vStrValues) {
    WCHAR* ptrValues = createDoubleNulTermList(vStrValues);
    int cch = 1;
    int n = vStrValues.size();

    for (int i = 0; i < n; i++)
        cch += vStrValues[i].length() + 1;

    HRESULT hr = setValue(pszValueName, REG_MULTI_SZ, (const LPBYTE)ptrValues, cch * sizeof(TCHAR));

    SAFE_DELETE_ARRAY(ptrValues);

    return hr;
}

HRESULT RegKey::getSubKeys(std::vector<std::wstring>& subKeys) {
    WCHAR achKey[256];               // buffer for subkey name
    DWORD cbName = 255;              // size of name string
    WCHAR achClass[MAX_PATH] = L"";  // buffer for class name
    DWORD cchClassName = MAX_PATH;   // size of class string
    DWORD cSubKeys = 0;              // number of subkeys
    DWORD cbMaxSubKey;               // longest subkey size
    DWORD cchMaxClass;               // longest class string
    DWORD cValues;                   // number of values for key
    DWORD cchMaxValue;               // longest value name
    DWORD cbMaxValueData;            // longest value data
    DWORD cbSecurityDescriptor;      // size of security descriptor
    FILETIME ftLastWriteTime;        // last write time

    DWORD retCode = RegQueryInfoKeyW(m_hkey,                 // key handle
                                     achClass,               // buffer for class name
                                     &cchClassName,          // size of class string
                                     NULL,                   // reserved
                                     &cSubKeys,              // number of subkeys
                                     &cbMaxSubKey,           // longest subkey size
                                     &cchMaxClass,           // longest class string
                                     &cValues,               // number of values for this key
                                     &cchMaxValue,           // longest value name
                                     &cbMaxValueData,        // longest value data
                                     &cbSecurityDescriptor,  // security descriptor
                                     &ftLastWriteTime);      // last write time

    if (retCode != ERROR_SUCCESS)
        return retCode;

    for (DWORD i = 0; i < cSubKeys; i++) {
        cbName = 255;
        retCode = RegEnumKeyExW(m_hkey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime);
        if (retCode == ERROR_SUCCESS) {
            subKeys.push_back(achKey);
        }
    }

    return ERROR_SUCCESS;
}

void RegKey::OnChange(HKEY hkey) {
    UNREFERENCED_PARAMETER(hkey);
    //
    // Default does nothing.
    //
}

HRESULT RegKey::getValue(LPCWSTR pszValueName, DWORD dwTypeExpected, LPBYTE pbData, DWORD cbData) const {
    DWORD dwType;
    HRESULT hr = RegQueryValueExW(m_hkey, pszValueName, 0, &dwType, pbData, (LPDWORD)&cbData);

    if (ERROR_SUCCESS == hr && dwType != dwTypeExpected)
        hr = ERROR_INVALID_DATATYPE;

    return hr;
}

HRESULT RegKey::setValue(LPCWSTR pszValueName, DWORD dwValueType, const LPBYTE pbData, int cbData) {
    HRESULT hr = RegSetValueExW(m_hkey, pszValueName, 0, dwValueType, pbData, cbData);

    return hr;
}

LPWSTR RegKey::createDoubleNulTermList(const std::vector<std::wstring>& vStrValues) const {
    size_t cEntries = vStrValues.size();
    size_t cch = 1;  // Account for 2nd null terminate.

    for (size_t i = 0; i < cEntries; i++)
        cch += vStrValues[i].length() + 1;

    LPWSTR pszBuf = new WCHAR[cch];
    LPWSTR pszWrite = pszBuf;

    for (size_t i = 0; i < cEntries; i++) {
        const std::wstring& s = vStrValues[i];
        StringCchCopyW(pszWrite, cch, s.c_str());
        pszWrite += s.length() + 1;
    }

    *pszWrite = L'\0';  // Double null terminate.
    return pszBuf;
}

unsigned int _stdcall RegKey::NotifyWaitThreadProc(LPVOID pvParam) {
    RegKey* pThis = (RegKey*)pvParam;

    while (NULL != pThis->m_hkey) {
        LONG lResult = RegNotifyChangeKeyValue(pThis->m_hkey, pThis->m_bWatchSubtree,
                                               pThis->m_dwChangeFilter, pThis->m_hChangeEvent, true);

        if (ERROR_SUCCESS != lResult) {
            return 0;
        }
        else {
            switch (WaitForSingleObject(pThis->m_hChangeEvent, INFINITE)) {
                case WAIT_OBJECT_0:
                    if (NULL != pThis->m_hkey) {
                        pThis->OnChange(pThis->m_hkey);
                    }

                    break;

                case WAIT_FAILED:
                    break;

                default:
                    break;
            }
        }
    }

    return 0;
}

bool RegKey::regDeleteKey32_64(HKEY hKey, LPCWSTR pszSubKey, bool bPrefer64View) {
    REGSAM rsam = (bPrefer64View && OSVersion::IsWin64()) ? KEY_WOW64_64KEY : KEY_WOW64_32KEY;
    HMODULE hAdvAPI32 = LoadLibrary(TEXT("AdvAPI32.dll"));

    if (!hAdvAPI32)
        return false;

    LSTATUS ls;
    typedef LONG(WINAPI * PFN_RegDeleteKeyEx)(HKEY hKey, LPCWSTR lpSubKey, REGSAM samDesired,
                                              DWORD Reserved);
#if (defined UNICODE) || (defined _UNICODE)
#define RegDeleteKeyExFuncName "RegDeleteKeyExW"
#else
#define RegDeleteKeyExFuncName "RegDeleteKeyExA"
#endif
    PFN_RegDeleteKeyEx _RegDeleteKeyEx =
        (PFN_RegDeleteKeyEx)GetProcAddress(hAdvAPI32, RegDeleteKeyExFuncName);

    if (_RegDeleteKeyEx) {
        ls = _RegDeleteKeyEx(hKey, pszSubKey, rsam, 0);
        FreeLibrary(hAdvAPI32);
    }
    else {
        ls = RegDeleteKeyW(hKey, pszSubKey);
    }

    return (ls == ERROR_SUCCESS);
}

bool RegKey::regDeleteSubKeys(HKEY hKey, bool bPrefer64View) {
    DWORD dwSubKeyCnt, dwMaxSubKey;
    const int iMaxKeySize = 256;

    if (RegQueryInfoKeyW(hKey, NULL, NULL, 0, &dwSubKeyCnt, &dwMaxSubKey, NULL, NULL, NULL, NULL,
                         NULL, NULL) == ERROR_SUCCESS) {
        if (dwSubKeyCnt) {
            dwMaxSubKey += sizeof(WCHAR);
            WCHAR szKeyName[iMaxKeySize] = {0};

            do {
                if (RegEnumKeyW(hKey, --dwSubKeyCnt, szKeyName, iMaxKeySize) == ERROR_SUCCESS) {
                    HKEY hSubKey = NULL;

                    if (ERROR_SUCCESS ==
                            RegOpenKeyExW(hKey, szKeyName, 0, KEY_READ | KEY_WRITE, &hSubKey) &&
                        hSubKey) {
                        if (regDeleteSubKeys(hSubKey, bPrefer64View)) {
                            if (hSubKey)
                                RegCloseKey(hSubKey);

                            if (!regDeleteKey32_64(hKey, szKeyName, bPrefer64View))
                                return false;
                        }
                        else {
                            if (hSubKey)
                                RegCloseKey(hSubKey);

                            return false;
                        }
                    }
                    else {
                        return false;
                    }
                }
                else {
                    return false;
                }
            } while (dwSubKeyCnt);
        }

        return true;
    }

    return false;
}

BOOL RegKey::regDelSubKeysRecurse(HKEY hKeyRoot, LPTSTR lpSubKey, bool bPrefer64View) {
    LPTSTR lpEnd = NULL;
    LONG lResult;
    DWORD dwSize = 0;
    TCHAR szName[MAX_PATH] = {0};
    HKEY hKey = NULL;
    FILETIME ftWrite;

    // First, see if we can delete the key without having to recurse.
    lResult =
        RegDeleteKeyEx(hKeyRoot, lpSubKey, bPrefer64View ? KEY_WOW64_64KEY : KEY_WOW64_32KEY, 0);
    if (lResult == ERROR_SUCCESS)
        return TRUE;

    REGSAM rsam = KEY_READ;
    if (bPrefer64View)
        rsam |= KEY_WOW64_64KEY;

    lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, rsam, &hKey);
    if (lResult != ERROR_SUCCESS) {
        if (lResult == ERROR_FILE_NOT_FOUND) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    }

    // Check for an ending slash and add one if it is missing.

    lpEnd = lpSubKey + lstrlen(lpSubKey);

    if (*(lpEnd - 1) != TEXT('\\')) {
        *lpEnd = TEXT('\\');
        lpEnd++;
        *lpEnd = TEXT('\0');
    }

    // Enumerate the keys
    dwSize = MAX_PATH;
    lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite);
    if (lResult == ERROR_SUCCESS) {
        do {
            *lpEnd = TEXT('\0');
            StringCchCat(lpSubKey, MAX_PATH * 2, szName);

            if (!regDelSubKeysRecurse(hKeyRoot, lpSubKey, bPrefer64View)) {
                break;
            }

            dwSize = MAX_PATH;
            lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite);
        } while (lResult == ERROR_SUCCESS);
    }

    lpEnd--;
    *lpEnd = TEXT('\0');

    RegCloseKey(hKey);

    // Try again to delete the key.
    lResult =
        RegDeleteKeyEx(hKeyRoot, lpSubKey, bPrefer64View ? KEY_WOW64_64KEY : KEY_WOW64_32KEY, 0);
    if (lResult == ERROR_SUCCESS)
        return TRUE;

    return FALSE;
}
}  // namespace jhc
#endif