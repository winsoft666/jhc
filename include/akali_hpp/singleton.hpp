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

#ifndef AKALI_SINGLETON_HPP_
#define AKALI_SINGLETON_HPP_

#include "akali_hpp/arch.hpp"
#include <mutex>
#ifdef AKALI_WIN
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
#include "akali_hpp/macros.hpp"

namespace akali_hpp {
template <class T>
class Singleton {
 public:
  static T* Instance();
  static void Release();

 protected:
  Singleton() {}
  Singleton(const Singleton&) {}
  Singleton& operator=(const Singleton&) {}

 private:
  static T* this_;
  static std::mutex m_;
};

template <class T>
T* Singleton<T>::this_ = nullptr;

template <class T>
std::mutex Singleton<T>::m_;

template <class T>
T* Singleton<T>::Instance(void) {
  // double-check
  if (this_ == nullptr) {
    std::lock_guard<std::mutex> lg(m_);
    if (this_ == nullptr) {
      this_ = new T;
    }
  }
  return this_;
}

template <class T>
void Singleton<T>::Release(void) {
  if (this_) {
    delete this_;
  }
}

#define SINGLETON_CLASS_DECLARE(class_name) friend class ::akali_hpp::Singleton<##class_name>;

class SingletonProcess {
 public:
#ifdef AKALI_WIN
  SingletonProcess(const std::string& unique_name) : unique_name_(unique_name) {}
#else
  SingletonProcess(const std::string& unique_pid_name) : unique_pid_name_(unique_pid_name) {}
#endif
  ~SingletonProcess() = default;
  bool operator()() {
#ifdef AKALI_WIN
    HANDLE mutex = CreateEventA(NULL, TRUE, FALSE, unique_name_.c_str());
    DWORD gle = GetLastError();
    bool ret = true;

    if (mutex) {
      if (gle == ERROR_ALREADY_EXISTS) {
        CloseHandle(mutex);
        ret = false;
      }
    }
    else {
      if (gle == ERROR_ACCESS_DENIED)
        ret = false;
    }

    return ret;
#else
    int pid_file = open(("/tmp/" + unique_pid_name_ + ".pid").c_str(), O_CREAT | O_RDWR, 0666);
    int rc = flock(pid_file, LOCK_EX | LOCK_NB);
    if (rc) {
      if (EWOULDBLOCK == errno)
        return false;
    }
    return true;
#endif
  }

 private:
#ifdef AKALI_WIN
  std::string unique_name_;
#else
  std::string unique_pid_name_;
#endif
  AKALI_DISALLOW_IMPLICIT_CONSTRUCTORS(SingletonProcess);
};

}  // namespace akali_hpp
#endif  // !AKALI_SINGLETON_HPP_
