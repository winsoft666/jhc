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

#ifndef JHC_FILE_INFO_HPP__
#define JHC_FILE_INFO_HPP__

#include "jhc/arch.hpp"
#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif // !_INC_WINDOWS
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

#pragma comment(lib, "Version.lib")
#endif

#include <string>

namespace jhc {
#ifdef JHC_WIN
class WinFileInfo {
   public:
    WinFileInfo() {
        reset();
    }

    ~WinFileInfo() = default;

    bool create(HMODULE hModule) {
        WCHAR szBuf[MAX_PATH + 1] = {0};
        GetModuleFileNameW(hModule, szBuf, MAX_PATH);

        return create(szBuf);
    }

    bool create(const std::wstring& strFileName) {
        reset();

        HANDLE hFile = CreateFileW(strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            return false;
        }

        BOOL bRet = GetFileTime(hFile, &m_ftCreationTime, &m_ftLastAccessTime, &m_ftLastWriteTime);
        if (!bRet) {
            CloseHandle(hFile);
            return false;
        }
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;

        DWORD dwHandle = 0;
        DWORD dwFileVersionInfoSize = GetFileVersionInfoSizeW(strFileName.c_str(), &dwHandle);

        if (!dwFileVersionInfoSize)
            return false;

        LPVOID lpData = (LPVOID)(new BYTE[dwFileVersionInfoSize]);

        if (!lpData)
            return false;

        try {
            if (!GetFileVersionInfoW((LPWSTR)strFileName.c_str(), dwHandle, dwFileVersionInfoSize, lpData))
                return false;

            LPVOID lpInfo;
            UINT unInfoLen;

            if (VerQueryValueW(lpData, L"\\", &lpInfo, &unInfoLen)) {
                if (unInfoLen == sizeof(m_FileInfo))
                    memcpy(&m_FileInfo, lpInfo, unInfoLen);
            }

            VerQueryValueW(lpData, L"\\VarFileInfo\\Translation", &lpInfo, &unInfoLen);

            DWORD dwLangCode = 0;

            if (!GetTranslationId(lpInfo, unInfoLen, GetUserDefaultLangID(), dwLangCode, FALSE)) {
                if (!GetTranslationId(lpInfo, unInfoLen, GetUserDefaultLangID(), dwLangCode, TRUE)) {
                    if (!GetTranslationId(lpInfo, unInfoLen, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                                          dwLangCode, TRUE)) {
                        if (!GetTranslationId(lpInfo, unInfoLen, MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL),
                                              dwLangCode, TRUE))
                            // use the first one we can get
                            dwLangCode = *((DWORD*)lpInfo);
                    }
                }
            }

            WCHAR strSubBlock[MAX_PATH * 2] = {0};
            ZeroMemory(strSubBlock, MAX_PATH * 2 * sizeof(WCHAR));
            swprintf_s(strSubBlock, L"\\StringFileInfo\\%04X%04X\\", dwLangCode & 0x0000FFFF,
                       (dwLangCode & 0xFFFF0000) >> 16);

            WCHAR strTmp[MAX_PATH * 2] = {0};
            ZeroMemory(strTmp, MAX_PATH * 2 * sizeof(WCHAR));

            swprintf_s(strTmp, L"%s%s", strSubBlock, L"CompanyName");

            if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
                m_strCompanyName = (LPCWSTR)lpInfo;

            swprintf_s(strTmp, L"%s%s", strSubBlock, L"FileDescription");

            if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
                m_strFileDescription = (LPCWSTR)lpInfo;

            swprintf_s(strTmp, L"%s%s", strSubBlock, L"FileVersion");

            if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
                m_strFileVersion = (LPCWSTR)lpInfo;

            swprintf_s(strTmp, L"%s%s", strSubBlock, L"InternalName");

            if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
                m_strInternalName = (LPCWSTR)lpInfo;

            swprintf_s(strTmp, L"%s%s", strSubBlock, L"LegalCopyright");

            if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
                m_strLegalCopyright = (LPCWSTR)lpInfo;

            swprintf_s(strTmp, L"%s%s", strSubBlock, L"OriginalFileName");

            if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
                m_strOriginalFileName = (LPCWSTR)lpInfo;

            swprintf_s(strTmp, L"%s%s", strSubBlock, L"ProductName");

            if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
                m_strProductName = (LPCWSTR)lpInfo;

            swprintf_s(strTmp, L"%s%s", strSubBlock, L"ProductVersion");

            if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
                m_strProductVersion = (LPCWSTR)lpInfo;

            swprintf_s(strTmp, L"%s%s", strSubBlock, L"Comments");

            if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
                m_strComments = (LPCWSTR)lpInfo;

            swprintf_s(strTmp, L"%s%s", strSubBlock, L"LegalTrademarks");

            if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
                m_strLegalTrademarks = (LPCWSTR)lpInfo;

            swprintf_s(strTmp, L"%s%s", strSubBlock, L"PrivateBuild");

            if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
                m_strPrivateBuild = (LPCWSTR)lpInfo;

            swprintf_s(strTmp, L"%s%s", strSubBlock, L"SpecialBuild");

            if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
                m_strSpecialBuild = (LPCWSTR)lpInfo;

            delete[] lpData;
        } catch (...) {
            delete[] lpData;
            return false;
        }

        return true;
    }

    WORD getFileVersion(int nIndex) const {
        switch (nIndex) {
            case 0:
                return (WORD)(m_FileInfo.dwFileVersionLS & 0x0000FFFF);

            case 1:
                return (WORD)((m_FileInfo.dwFileVersionLS & 0xFFFF0000) >> 16);

            case 2:
                return (WORD)(m_FileInfo.dwFileVersionMS & 0x0000FFFF);

            case 3:
                return (WORD)((m_FileInfo.dwFileVersionMS & 0xFFFF0000) >> 16);

            default:
                return 0;
        }
    }

    WORD getProductVersion(int nIndex) const {
        switch (nIndex) {
            case 0:
                return (WORD)(m_FileInfo.dwProductVersionLS & 0x0000FFFF);

            case 1:
                return (WORD)((m_FileInfo.dwProductVersionLS & 0xFFFF0000) >> 16);

            case 2:
                return (WORD)(m_FileInfo.dwProductVersionMS & 0x0000FFFF);

            case 3:
                return (WORD)((m_FileInfo.dwProductVersionMS & 0xFFFF0000) >> 16);

            default:
                return 0;
        }
    }

    DWORD getFileFlagsMask() const {
        return m_FileInfo.dwFileFlagsMask;
    }

    DWORD getFileFlags() const {
        return m_FileInfo.dwFileFlags;
    }

    DWORD getFileOs() const {
        return m_FileInfo.dwFileOS;
    }

    DWORD getFileType() const {
        return m_FileInfo.dwFileType;
    }

    DWORD getFileSubtype() const {
        return m_FileInfo.dwFileSubtype;
    }

    std::wstring getCompanyName() const {
        return m_strCompanyName;
    }

    std::wstring getFileDescription() const {
        return m_strFileDescription;
    }

    std::wstring getFileVersion() const {
        return m_strFileVersion;
    }

    std::wstring getFileVersionEx() const {
        wchar_t strTemp[MAX_PATH * 2] = {0};

        StringCchPrintfW(
            strTemp, MAX_PATH * 2, L"%d.%d.%d.%d", (m_FileInfo.dwFileVersionMS & 0xFFFF0000) >> 16,
            (m_FileInfo.dwFileVersionMS & 0x0000FFFF), (m_FileInfo.dwFileVersionLS & 0xFFFF0000) >> 16,
            m_FileInfo.dwFileVersionLS & 0x0000FFFF);

        return strTemp;
    }

    std::wstring getInternalName() const {
        return m_strInternalName;
    }

    std::wstring getLegalCopyright() const {
        return m_strLegalCopyright;
    }

    std::wstring getOriginalFileName() const {
        return m_strOriginalFileName;
    }

    std::wstring getProductName() const {
        return m_strProductName;
    }

    std::wstring getProductVersion() const {
        return m_strProductVersion;
    }

    std::wstring getComments() const {
        return m_strComments;
    }

    std::wstring getLegalTrademarks() const {
        return m_strLegalTrademarks;
    }

    std::wstring getPrivateBuild() const {
        return m_strPrivateBuild;
    }

    std::wstring getSpecialBuild() const {
        return m_strSpecialBuild;
    }

    // Windows的文件时间为一个64位整数（用FILETIME结构体存储）,它记录从1601-1-1
    // 00:00:00到当前格林威治时间（UTC）所经过的100纳秒(ns)数 See: https://blog.csdn.net/china_jeffery/article/details/78409614
    //
    FILETIME getCreationTime() const {
        return m_ftCreationTime;
    }

    FILETIME getLastAccessTime() const {
        return m_ftLastAccessTime;
    }

    FILETIME getLastWriteTime() const {
        return m_ftLastWriteTime;
    }

   private:
    static bool GetTranslationId(LPVOID lpData,
                          UINT unBlockSize,
                          WORD wLangId,
                          DWORD& dwId,
                          BOOL bPrimaryEnough = FALSE) {
        LPWORD lpwData;

        for (lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData) + unBlockSize; lpwData += 2) {
            if (*lpwData == wLangId) {
                dwId = *((DWORD*)lpwData);
                return true;
            }
        }

        if (!bPrimaryEnough)
            return false;

        for (lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData) + unBlockSize; lpwData += 2) {
            if (((*lpwData) & 0x00FF) == (wLangId & 0x00FF)) {
                dwId = *((DWORD*)lpwData);
                return true;
            }
        }

        return false;
    }

    void reset() {
        ZeroMemory(&m_FileInfo, sizeof(m_FileInfo));
        m_strCompanyName.clear();
        m_strFileDescription.clear();
        m_strFileVersion.clear();
        m_strInternalName.clear();
        m_strLegalCopyright.clear();
        m_strOriginalFileName.clear();
        m_strProductName.clear();
        m_strProductVersion.clear();
        m_strComments.clear();
        m_strLegalTrademarks.clear();
        m_strPrivateBuild.clear();
        m_strSpecialBuild.clear();

        m_ftCreationTime.dwHighDateTime = m_ftCreationTime.dwLowDateTime = 0;
        m_ftLastAccessTime.dwHighDateTime = m_ftLastAccessTime.dwLowDateTime = 0;
        m_ftLastWriteTime.dwHighDateTime = m_ftLastWriteTime.dwLowDateTime = 0;
    }

   private:
    VS_FIXEDFILEINFO m_FileInfo;

    std::wstring m_strCompanyName;
    std::wstring m_strFileDescription;
    std::wstring m_strFileVersion;
    std::wstring m_strInternalName;
    std::wstring m_strLegalCopyright;
    std::wstring m_strOriginalFileName;
    std::wstring m_strProductName;
    std::wstring m_strProductVersion;
    std::wstring m_strComments;
    std::wstring m_strLegalTrademarks;
    std::wstring m_strPrivateBuild;
    std::wstring m_strSpecialBuild;

    FILETIME m_ftCreationTime;
    FILETIME m_ftLastAccessTime;
    FILETIME m_ftLastWriteTime;
};
#endif
}  // namespace jhc
#endif