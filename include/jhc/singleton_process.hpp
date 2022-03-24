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
#include "jhc/macros.hpp"
#include "jhc/uuid.hpp"

namespace jhc {
    class SingletonProcess {
    public:
        SingletonProcess() {
            uniqueName_ = jhc::UUID::Create();
            assert(!uniqueName_.empty());
            check();
        }

        SingletonProcess(const std::string& uniqueName) : uniqueName_(uniqueName) {
            assert(!uniqueName_.empty());
            check();
        }

        SingletonProcess(std::string&& uniqueName) : uniqueName_(std::move(uniqueName)) {
            assert(!uniqueName_.empty());
            check();
        }

        SingletonProcess(const SingletonProcess& that) {
            uniqueName_ = that.uniqueName_;
            isPrimary_ = that.isPrimary_;
        }

        SingletonProcess(SingletonProcess&& that) {
            uniqueName_ = std::move(that.uniqueName_);
            isPrimary_ = that.isPrimary_;
        }

        ~SingletonProcess() = default;

        SingletonProcess& operator=(const SingletonProcess& that) {
            uniqueName_ = that.uniqueName_;
            isPrimary_ = that.isPrimary_;
            return *this;
        }

        SingletonProcess& operator=(SingletonProcess&& that) {
            uniqueName_ = std::move(that.uniqueName_);
            isPrimary_ = that.isPrimary_;
            return *this;
        }

        const std::string& uniqueName() const {
            return uniqueName_;
        }

        bool operator()() const {
            return isPrimary_;
        }
    protected:
        void check() {
#ifdef JHC_WIN
            HANDLE mutex = CreateEventA(NULL, TRUE, FALSE, uniqueName_.c_str());
            const DWORD gle = GetLastError();
            isPrimary_ = true;

            if (mutex) {
                if (gle == ERROR_ALREADY_EXISTS) {
                    CloseHandle(mutex);
                    isPrimary_ = false;
                }
            }
            else {
                if (gle == ERROR_ACCESS_DENIED)
                    isPrimary_ = false;
            }
            // NOT CloseHandle
#else
            int pid_file = open(("/tmp/" + uniqueName_ + ".pid").c_str(), O_CREAT | O_RDWR, 0666);
            int rc = flock(pid_file, LOCK_EX | LOCK_NB);
            if (rc) {
                if (EWOULDBLOCK == errno)
                    isPrimary_ = false;
            }
            isPrimary_ = true;
#endif
        }

    private:
        std::string uniqueName_;
        bool isPrimary_ = true;
    };
} // namespace jhc 
#endif // !JHC_SINGLETON_PROCESS_HPP__
