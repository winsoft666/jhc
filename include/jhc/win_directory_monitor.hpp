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

#ifndef JHC_WIN_DIRECTORY_MONITOR_HPP__
#define JHC_WIN_DIRECTORY_MONITOR_HPP__

#include "jhc/arch.hpp"

#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif
#include <string>
#include <functional>
#include <future>
#include <vector>
#include "jhc/macros.hpp"

namespace jhc {
class WinDirectoryMonitor {
   public:
    typedef std::function<void(DWORD /*action*/, std::wstring /*path*/)> ChangeNotify;
    WinDirectoryMonitor() :
        directory_(INVALID_HANDLE_VALUE) {
        ::ZeroMemory(&overlapped_, sizeof(OVERLAPPED));
        overlapped_.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    virtual ~WinDirectoryMonitor() {
        if (monitorTask_.valid())
            monitorTask_.get();

        SAFE_CLOSE(overlapped_.hEvent);
        ::ZeroMemory(&overlapped_, sizeof(OVERLAPPED));
    }

    bool startMonitor(const std::wstring& directory,
                      bool watch_sub_dir,
                      DWORD filter,
                      ChangeNotify notifyCallback) {
        std::wstring strDirectoryFormat = directory;
        if (strDirectoryFormat.length() > 0) {
            if (strDirectoryFormat[strDirectoryFormat.length() - 1] != L'\\')
                strDirectoryFormat += L"\\";
        }

        SAFE_CLOSE_ON_VALID_HANDLE(directory_);

        directory_ = ::CreateFileW(strDirectoryFormat.c_str(), FILE_LIST_DIRECTORY,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
                                   FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

        if (directory_ == INVALID_HANDLE_VALUE) {
            return false;
        }

        int watch_buffer_size = 1024;
        void* watch_buffer = malloc(watch_buffer_size);
        if (!watch_buffer) {
            SAFE_CLOSE_ON_VALID_HANDLE(directory_);
            return false;
        }
        memset(watch_buffer, 0, watch_buffer_size);

        DWORD has_read = 0;
        const BOOL success = ::ReadDirectoryChangesW(directory_, watch_buffer, watch_buffer_size, watch_sub_dir,
                                                     filter, &has_read, &overlapped_, NULL);
        if (!success) {
            SAFE_FREE(watch_buffer);
            SAFE_CLOSE_ON_VALID_HANDLE(directory_);
            return false;
        }

        monitorTask_ = std::async(std::launch::async, [watch_buffer, watch_buffer_size, filter, notifyCallback,
                                                       strDirectoryFormat, watch_sub_dir, this]() {
            do {
                DWORD transferred = 0;
                BOOL bRet = GetOverlappedResult(directory_, &overlapped_, &transferred, TRUE);
                DWORD gle = GetLastError();
                if (bRet && gle == 0) {
                    BYTE* base = (BYTE*)watch_buffer;
                    for (;;) {
                        FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*)base;
                        if (notifyCallback) {
                            std::wstring str_path;
                            str_path.assign(fni->FileName, fni->FileNameLength / sizeof(WCHAR));
                            notifyCallback(fni->Action, strDirectoryFormat + str_path);
                        }
                        if (!fni->NextEntryOffset)
                            break;
                        base += fni->NextEntryOffset;
                    }

                    DWORD hasRead = 0;
                    const BOOL success = ::ReadDirectoryChangesW(directory_, watch_buffer, watch_buffer_size, watch_sub_dir,
                                                                 filter, &hasRead, &overlapped_, NULL);
                    if (!success) {
                        SAFE_CLOSE_ON_VALID_HANDLE(directory_);
                        return;
                    }
                }
                else {
                    if (watch_buffer)
                        free(watch_buffer);
                    break;
                }
            } while (true);
        });

        return true;
    }

    void stopMonitor() {
        if (directory_ != INVALID_HANDLE_VALUE) {
            CancelIo(directory_);
            ::CloseHandle(directory_);
            directory_ = INVALID_HANDLE_VALUE;
        }
    }

   private:
    HANDLE directory_;
    OVERLAPPED overlapped_;
    std::future<void> monitorTask_;

    JHC_DISALLOW_COPY_AND_ASSIGN(WinDirectoryMonitor);
};
}  // namespace jhc
#endif
#endif  // !JHC_WIN_DIRECTORY_MONITOR_HPP__