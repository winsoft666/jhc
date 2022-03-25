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
#ifndef JHC_SINGLETON_PROCESS_HPP__
#define JHC_SINGLETON_PROCESS_HPP__
#include "jhc/arch.hpp"
#ifdef JHC_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#endif
#include "jhc/string_helper.hpp"
#include "jhc/singleton_class.hpp"

namespace jhc {
class SingletonProcess : public SingletonClass<SingletonProcess> {
   public:
    void markProcessStartup(const std::string& uniqueName) {
        if (uniqueName_.empty() && !uniqueName.empty()) {
            uniqueName_ = uniqueName;
            check();
        }
    }

    const std::string& uniqueName() const {
        return uniqueName_;
    }

    bool isPrimary() const {
        assert(!uniqueName_.empty());
        return isPrimary_;
    }

   protected:
    void check() {
#ifdef JHC_WIN
        if(StringHelper::IsStartsWith(uniqueName_, "Global\\"))
            mutex_ = CreateEventA(NULL, TRUE, FALSE, uniqueName_.c_str());
        else
            mutex_ = CreateEventA(NULL, TRUE, FALSE, ("Global\\" + uniqueName_).c_str());

        const DWORD gle = GetLastError();
        isPrimary_ = true;

        if (mutex_) {
            if (gle == ERROR_ALREADY_EXISTS) {
                isPrimary_ = false;
            }
        }
        else {
            if (gle == ERROR_ACCESS_DENIED)
                isPrimary_ = false;
        }
#else
        pidFile_ = open(("/tmp/" + uniqueName_ + ".pid").c_str(), O_CREAT | O_RDWR, 0666);
        int rc = flock(pidFile_, LOCK_EX | LOCK_NB);
        if (rc) {
            if (EWOULDBLOCK == errno)
                isPrimary_ = false;
        }
        isPrimary_ = true;
#endif
    }

   private:
    ~SingletonProcess() {
#ifdef JHC_WIN
        if(mutex_) {
            CloseHandle(mutex_);
            mutex_ = NULL;
        }
#else
        if(pidFile_ != -1) {
            close(pidFile_);
            pidFile_ = -1;
        }
#endif
    }

    std::string uniqueName_;
    bool isPrimary_ = true;
#ifdef JHC_WIN
    HANDLE mutex_ = NULL;
#else
    int pidFile_ = -1;
#endif
    JHC_SINGLETON_CLASS_DECLARE(SingletonProcess);
};
}  // namespace jhc
#endif  // !JHC_SINGLETON_PROCESS_HPP__
