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

#ifndef AKALI_FILE_UTIL_HPP_
#define AKALI_FILE_UTIL_HPP_

#include "akali_hpp/arch.hpp"

#ifdef AKALI_WIN
#include "akali_hpp/string_encode.hpp"
#include <io.h>
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif  // !_INC_WINDOWS
#include <strsafe.h>
#include <Shlwapi.h>
#endif  // !AKALI_WIN
#include "akali_hpp/path_util.hpp"

namespace akali_hpp {
class FileUtil {
   public:
    static void CopyFolder(const PathString& from, const PathString& to, bool bCopySource, int* pIgnoredNum) {
#ifdef AKALI_WIN
        wchar_t szFrom[MAX_PATH] = {0};
        wchar_t szTo[MAX_PATH] = {0};
        wcscpy_s(szFrom, MAX_PATH, from.c_str());
        wcscpy_s(szTo, MAX_PATH, to.c_str());

        if (bCopySource) {
            WIN32_FIND_DATAW filedata;
            HANDLE fhandle = FindFirstFileW(szFrom, &filedata);
            size_t len = wcslen(szTo);
            if (szTo[len - 1] != L'\\')
                wcscat_s(szTo, MAX_PATH, L"\\");
            wcscat_s(szTo, filedata.cFileName);
            ::CreateDirectoryW(szTo, NULL);
        }

        WinFileRecurveSearch(szFrom, szTo, pIgnoredNum);
#else
# warning Not Implemented
#endif
    }

    static bool DeleteFolder(const PathString& folder) {
#ifdef AKALI_WIN
        if (folder.empty())
            return false;

        bool bRet = true;
        const int kBufSize = MAX_PATH * 4;
        HANDLE hFind = INVALID_HANDLE_VALUE;
        WCHAR szTemp[kBufSize] = {0};
        WIN32_FIND_DATAW wfd;

        StringCchCopyW(szTemp, kBufSize, folder.c_str());
        PathAddBackslashW(szTemp);
        StringCchCatW(szTemp, kBufSize, L"*.*");

        hFind = FindFirstFileW(szTemp, &wfd);
        if (hFind == INVALID_HANDLE_VALUE) {
            return false;
        }

        do {
            if (lstrcmpiW(wfd.cFileName, L".") != 0 && lstrcmpiW(wfd.cFileName, L"..") != 0) {
                StringCchCopyW(szTemp, kBufSize, folder.c_str());
                PathAddBackslashW(szTemp);
                StringCchCatW(szTemp, kBufSize, wfd.cFileName);

                if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    bRet = DeleteFolder(szTemp);
                }
                else {
                    bRet = (DeleteFileW(szTemp) == TRUE);
                    if (!bRet) {
                        SetFileAttributesW(szTemp, FILE_ATTRIBUTE_NORMAL);
                        bRet = (DeleteFileW(szTemp) == TRUE);
                    }
                }
            }

        } while (FindNextFileW(hFind, &wfd));

        FindClose(hFind);

        if (!bRet) {
            return bRet;
        }

        bRet = (::RemoveDirectoryW(folder.c_str()) == TRUE);
        if (!bRet) {
            DWORD dwAttr = ::GetFileAttributesW(folder.c_str());
            dwAttr &= ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN);
            ::SetFileAttributesW(folder.c_str(), dwAttr);
            bRet = (::RemoveDirectoryW(folder.c_str()) == TRUE);
        }

        return bRet;
#else
#warning Not Implemented
#endif
    }

    static bool CreateFolder(const PathString& folder) {
#ifdef AKALI_WIN
        if (folder.empty())
            return false;

        wchar_t* p = NULL;
        wchar_t* szDirBuf = NULL;
        DWORD dwAttributes;
        size_t iLen = folder.length();

        __try {
            szDirBuf = (wchar_t*)malloc((iLen + 1) * sizeof(wchar_t));
            if (szDirBuf == NULL)
                return false;

            StringCchCopyW(szDirBuf, iLen + 1, folder.c_str());
            p = szDirBuf;

            if ((*p == L'\\') && (*(p + 1) == L'\\')) {
                p += 2;
                while (*p && *p != L'\\')
                    p = CharNextW(p);
                if (*p)
                    p++;
                while (*p && *p != L'\\')
                    p = CharNextW(p);
                if (*p)
                    p++;
            }
            else if (*(p + 1) == L':') {
                p += 2;
                if (*p && (*p == L'\\'))
                    p++;
            }

            while (*p) {
                if (*p == L'\\') {
                    *p = L'\0';
                    dwAttributes = ::GetFileAttributesW(szDirBuf);

                    if (dwAttributes == 0xffffffff) {
                        if (!::CreateDirectoryW(szDirBuf, NULL)) {
                            if (GetLastError() != ERROR_ALREADY_EXISTS) {
                                free(szDirBuf);
                                return false;
                            }
                        }
                    }
                    else {
                        if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
                            free(szDirBuf);
                            return false;
                        }
                    }

                    *p = L'\\';
                }

                p = CharNextW(p);
            }
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            if (szDirBuf)
                free(szDirBuf);
            return false;
        }

        if (szDirBuf)
            free(szDirBuf);
        return true;
#else
#warning Not Implemented
#endif
    }

   private:
#ifdef AKALI_WIN
    static void WinFileCopy(const wchar_t* szPath, const wchar_t* szDest, WIN32_FIND_DATAW file, int* pIgnoredNum) {
        if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (((wcscmp(file.cFileName, L".") != 0) && (wcscmp(file.cFileName, L"..") != 0))) {
                wchar_t szTemp[MAX_PATH] = {0};
                wcscpy_s(szTemp, MAX_PATH, szPath);
                if (szTemp[wcslen(szTemp) - 1] != L'\\')
                    wcscat_s(szTemp, MAX_PATH, L"\\");

                wcscat_s(szTemp, MAX_PATH, file.cFileName);
                // create directory.
                wchar_t szDir[MAX_PATH] = {0};
                wcscat_s(szDir, MAX_PATH, szDest);
                wcscat_s(szDir, MAX_PATH, L"\\");
                wcscat_s(szDir, MAX_PATH, file.cFileName);
                if (_waccess_s(szDir, 0) != 0) {
                    if (!CreateDirectoryW(szDir, NULL)) {
                        if (pIgnoredNum) {
                            *pIgnoredNum += 1;
                        }
                    }
                }

                WinFileRecurveSearch(szTemp, szDir, pIgnoredNum);
            }
        }
        else {
            wchar_t szS[MAX_PATH] = {0};
            StringCchPrintfW(szS, MAX_PATH, L"%s\\%s", szPath, file.cFileName);
            wchar_t szD[MAX_PATH] = {0};
            StringCchPrintfW(szD, MAX_PATH, L"%s\\%s", szDest, file.cFileName);
            if (!CopyFileW(szS, szD, false)) {
                if (pIgnoredNum) {
                    *pIgnoredNum += 1;
                }
            }
        }
    }

    static void WinFileRecurveSearch(const wchar_t* szPath, const wchar_t* szDest, int* pIgnoredNum) {
        wchar_t szTemp[MAX_PATH] = {0};
        wcscpy_s(szTemp, MAX_PATH, szPath);

        size_t len = wcslen(szPath);
        if (szTemp[len - 1] != '\\')
            wcscat_s(szTemp, MAX_PATH, L"\\");
        wcscat_s(szTemp, MAX_PATH, L"*.*");

        WIN32_FIND_DATAW filedata;
        HANDLE fhandle = FindFirstFileW(szTemp, &filedata);
        if (fhandle != INVALID_HANDLE_VALUE) {
            WinFileCopy(szPath, szDest, filedata, pIgnoredNum);

            while (FindNextFileW(fhandle, &filedata) != 0) {
                WinFileCopy(szPath, szDest, filedata, pIgnoredNum);
            }
        }
        FindClose(fhandle);
    }
#endif
};
}  // namespace akali_hpp

#endif  // !AKALI_FILE_UTIL_HPP_
