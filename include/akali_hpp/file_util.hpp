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
#include <stdio.h>
#include <mutex>
#ifdef AKALI_WIN
#include <io.h>
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif  // !_INC_WINDOWS
#include <strsafe.h>
#include <Shlwapi.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif  // !AKALI_WIN
#include "akali_hpp/macros.hpp"
#include "akali_hpp/path_util.hpp"

namespace akali_hpp {
class File {
   public:
    File(const PathString& path) :
        path_(path) {
    }

    virtual ~File() {
        close();
    }

    PathString path() const {
        return path_;
    }

    bool isOpen() {
        std::lock_guard<std::recursive_mutex> lg(mutex_);
        return (f_ != nullptr);
    }

    // [r]  read
    //      Open file for input operations.
    //      The file must exist.
    //
    // [w]  write
    //      Create an empty file for output operations.
    //      If a file with the same name already exists, its contents are discarded and the file is treated as a new empty file.
    //
    // [a]  append
    //      Open file for output at the end of a file.
    //      Output operations always write data at the end of the file, expanding it.
    //      Repositioning operations (fseek, fsetpos, rewind) are ignored.
    //      The file is created if it does not exist.
    //
    // [r+] read/update
    //      Open a file for update (both for input and output).
    //      The file must exist.
    //
    // [w+] write/update
    //      Create an empty file and open it for update (both for input and output).
    //      If a file with the same name already exists its contents are discarded and the file is treated as a new empty file.
    //
    // [a+] append/update
    //      Open a file for update (both for input and output) with all output operations writing data at the end of the file.
    //      Repositioning operations (fseek, fsetpos, rewind) affects the next input operations,
    //          but output operations move the position back to the end of file.
    //      The file is created if it does not exist.
    //
    // [b]  binary mode
    //      With the mode specifiers above the file is open as a text file.
    //      In order to open a file as a binary file, a "b" character has to be included in the mode string.
    //      This additional "b" character can either be appended at the end of the string ("rb", "wb", "ab", "r+b", "w+b", "a+b")
    //         or be inserted between the letter and the "+" sign for the mixed modes ("rb+", "wb+", "ab+").
    //
    // see: https://www.cplusplus.com/reference/cstdio/fopen/
    //
    bool open(const PathString& openMode) {
        std::lock_guard<std::recursive_mutex> lg(mutex_);
        if (f_ != nullptr)
            return true;

        if (path_.empty() || openMode.empty())
            return false;

#ifdef AKALI_WIN
        _wfopen_s(&f_, path_.c_str(), openMode.c_str());
#else
        f_ = fopen(path_.c_str(), openMode.c_str());
#endif

        return (f_ != nullptr);
    }

    bool close() {
        std::lock_guard<std::recursive_mutex> lg(mutex_);
        if (f_) {
            const int err = fclose(f_);
            if (err == 0)
                f_ = nullptr;
            return (err == 0);
        }
        return false;
    }

    bool flush() {
        std::lock_guard<std::recursive_mutex> lg(mutex_);
        if (f_)
            return (fflush(f_) == 0);
        return false;
    }

    bool exist() const {
        if (path_.empty())
            return false;
#ifdef AKALI_WIN
        return (_waccess(path_.c_str(), 0) == 0);
#else
        return (access(path_.c_str(), F_OK) == 0);
#endif
    }

    bool canRW() const {
        if (path_.empty())
            return false;

#ifdef AKALI_WIN
        return (_waccess(path_.c_str(), 6) == 0);
#else
        return (access(path_.c_str(), R_OK | W_OK) == 0);
#endif
    }

    // Must be call open(...) first!
    // This function will NOT change file pointer position.
    // Return: < 0 failed
    //
    int64_t fileSize() {
        std::lock_guard<std::recursive_mutex> lg(mutex_);
        if (!f_)
            return -1;

#ifdef AKALI_WIN
        const int64_t curPos = _ftelli64(f_);
        if (curPos == -1L)
            return -1;

        if (_fseeki64(f_, 0L, SEEK_END) != 0)
            return -1;

        const int64_t ret = _ftelli64(f_);
        _fseeki64(f_, curPos, SEEK_SET);

        return ret;
#else
        const int64_t curPos = ftello64(f_);
        if (curPos == -1L)
            return -1;

        if (fseeko64(f_, 0L, SEEK_END) != 0)
            return -1;

        const int64_t ret = ftello64(f_);
        fseeko64(f_, curPos, SEEK_SET);

        return ret;
#endif
    }

    // Must be call open(...) first!
    bool seekFromCurrent(int64_t offset) {
        std::lock_guard<std::recursive_mutex> lg(mutex_);
        if (f_) {
#ifdef AKALI_WIN
            return (_fseeki64(f_, offset, SEEK_CUR) == 0);
#else
            return (fseeko64(f_, offset, SEEK_CUR) == 0);
#endif
        }
        return false;
    }

    // Return: -1 is failed
    int64_t currentPointerPos() {
        std::lock_guard<std::recursive_mutex> lg(mutex_);
        if (!f_)
            return -1;
#ifdef AKALI_WIN
        return _ftelli64(f_);
#else
        return ftello64(f_);
#endif
    }

    // Must be call open(...) first!
    bool seekFromBeginning(int64_t offset) {
        std::lock_guard<std::recursive_mutex> lg(mutex_);
        if (f_) {
#ifdef AKALI_WIN
            return (_fseeki64(f_, offset, SEEK_SET) == 0);
#else
            return (fseeko64(f_, offset, SEEK_SET) == 0);
#endif
        }
        return false;
    }

    // Must be call open(...) first!
    bool seekFromEnd(int64_t offset) {
        std::lock_guard<std::recursive_mutex> lg(mutex_);
        if (f_) {
#ifdef AKALI_WIN
            return (_fseeki64(f_, offset, SEEK_END) == 0);
#else
            return (fseeko64(f_, offset, SEEK_END) == 0);
#endif
        }
        return false;
    }

    // Must be call open(...) first!
    // This function will change file pointer position.
    // Caller need allocate/free buffer.
    size_t readFrom(void* buffer, size_t needRead, int64_t from = -1) {
        std::lock_guard<std::recursive_mutex> lg(mutex_);
        if (!f_ || !buffer || needRead == 0)
            return 0;

        if (from >= 0) {
#ifdef AKALI_WIN
            if (_fseeki64(f_, from, SEEK_SET) != 0)
                return 0;
#else
            if (fseeko64(f_, from, SEEK_SET) != 0)
                return 0;
#endif
        }

        const size_t read = fread(buffer, 1, needRead, f_);
        return read;
    }

    // Must be call open(...) first!
    // This function will change file pointer position.
    // Caller need allocate/free buffer.
    size_t writeFrom(void* buffer, size_t needWrite, int64_t from = -1) {
        std::lock_guard<std::recursive_mutex> lg(mutex_);
        if (!f_ || !buffer || needWrite == 0)
            return 0;

        if (from >= 0) {
#ifdef AKALI_WIN
            if (_fseeki64(f_, from, SEEK_SET) != 0)
                return 0;
#else
            if (fseeko64(f_, from, SEEK_SET) != 0)
                return 0;
#endif
        }

        const size_t written = fwrite(buffer, 1, needWrite, f_);
        return written;
    }

    // Must be call open(...) first!
    // This function will NOT change file pointer position.
    // readAll function will malloc memory, caller need free it.
    size_t readAll(void** buffer) {
        std::lock_guard<std::recursive_mutex> lg(mutex_);
        if (!f_ || !buffer)
            return 0;

#ifdef AKALI_WIN
        const int64_t curPos = _ftelli64(f_);
        if (curPos == -1L)
            return 0;

        if (_fseeki64(f_, 0L, SEEK_END) != 0)
            return 0;

        const int64_t fileSize = _ftelli64(f_);
        if (_fseeki64(f_, 0, SEEK_SET) != 0)
            return 0;

        size_t read = 0;
        if (fileSize <= SIZE_MAX) {
            *buffer = malloc((size_t)fileSize);
            if (*buffer != nullptr) {
                read = fread(*buffer, 1, (size_t)fileSize, f_);
            }
        }

        _fseeki64(f_, curPos, SEEK_SET);

        return read;
#else
        const int64_t curPos = ftello64(f_);
        if (curPos == -1L)
            return 0;

        if (fseeko64(f_, 0L, SEEK_END) != 0)
            return 0;

        const int64_t fileSize = ftello64(f_);
        if (fseeko64(f_, 0, SEEK_SET) != 0)
            return 0;

        size_t read = 0;
        if (fileSize <= SIZE_MAX) {
            *buffer = malloc((size_t)fileSize);
            if (*buffer != nullptr) {
                read = fread(*buffer, 1, (size_t)fileSize, f_);
            }
        }

        fseeko64(f_, curPos, SEEK_SET);

        return read;
#endif
    }

    // Must be call open(...) first!
    // This function will NOT change file pointer position.
    std::string readAll() {
        std::string ret;
        void* buffer = nullptr;
        const size_t read = readAll(&buffer);

        if (buffer) {
            ret.assign((const char* const)buffer, read);
            free(buffer);
        }
        return ret;
    }

   protected:
    FILE* f_ = nullptr;
    PathString path_;
    std::recursive_mutex mutex_;

    AKALI_DISALLOW_COPY_AND_ASSIGN(File);
};

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
#warning Not Implemented
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

    static bool IsExist(const PathString& path) {
        if (path.empty())
            return false;

#ifdef AKALI_WIN
        return (_waccess(path.c_str(), 0) == 0);
#else
        return (access(path.c_str(), F_OK) == 0);
#endif
    }

    // Delete file by path.
    static bool RemoveFile(const PathString& path) {
        if (path.empty())
            return false;

#ifdef AKALI_WIN
        return (_wremove(path.c_str()) == 0);
#else
        return (remove(path.c_str()) != 0);
#endif
    }

    static bool RenameFile(const PathString& from, const PathString& to) {
        if (from.empty() || to.empty())
            return false;

#ifdef AKALI_WIN
        return !!::MoveFileExW(from.c_str(), to.c_str(),
                             MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH | MOVEFILE_COPY_ALLOWED);
#else
        return 0 == rename(from.c_str(), to.c_str());
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
