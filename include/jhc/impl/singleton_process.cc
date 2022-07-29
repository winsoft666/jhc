#include "jhc/config.hpp"

#ifdef JHC_NOT_HEADER_ONLY
#include "../singleton_process.hpp"
#endif

#ifdef JHC_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#endif
#include <assert.h>
#include "jhc/string_helper.hpp"

namespace jhc {
JHC_INLINE void SingletonProcess::markAndCheckStartup(const std::string& uniqueName) {
    if (uniqueName_.empty() && !uniqueName.empty()) {
        uniqueName_ = uniqueName;
        check();
    }
}

JHC_INLINE const std::string& SingletonProcess::uniqueName() const {
    return uniqueName_;
}

JHC_INLINE bool SingletonProcess::isPrimary() const {
    assert(!uniqueName_.empty());
    return isPrimary_;
}

JHC_INLINE void SingletonProcess::check() {
#ifdef JHC_WIN
    if (StringHelper::IsStartsWith(uniqueName_, "Global\\"))
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

JHC_INLINE SingletonProcess::~SingletonProcess() {
#ifdef JHC_WIN
    if (mutex_) {
        CloseHandle(mutex_);
        mutex_ = NULL;
    }
#else
    if (pidFile_ != -1) {
        close(pidFile_);
        pidFile_ = -1;
    }
#endif
}

#ifdef JHC_WIN
JHC_INLINE void* SingletonProcess::mutex() const {
    return mutex_;
}
#else
JHC_INLINE int SingletonProcess::pidFile() const {
    return pidFile_;
}
#endif
}  // namespace jhc