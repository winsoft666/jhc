#include "jhc/config.hpp"

#ifdef JHC_NOT_HEADER_ONLY
#include "../thread.hpp"
#endif

#if defined JHC_WIN || defined JHC_LINUX
#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif
#elif (defined JHC_LINUX)
#include <sys/prctl.h>
#include <unistd.h>
#include <sys/syscall.h>
#endif

namespace jhc {
JHC_INLINE Thread::Thread() :
    thread_id_(0), exit_(false) {
    running_.store(false);
}

JHC_INLINE Thread::Thread(const std::string& name) :
    thread_id_(0), exit_(false), thread_name_(name) {
    running_.store(false);
}

JHC_INLINE Thread::~Thread() {
    stop(true);
}

JHC_INLINE void Thread::setName(const std::string& name) {
    thread_name_ = name;
}

JHC_INLINE std::string Thread::name() const {
    return thread_name_;
}

JHC_INLINE long Thread::id() {
    return thread_id_;
}

JHC_INLINE bool Thread::start() {
    if (thread_.valid()) {
        if (thread_.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
            return false;
        }
    }
    thread_ = std::async(std::launch::async, &Thread::run, this);
    return true;
}

JHC_INLINE void Thread::stop(bool wait_until_stopped) {
    {
        std::lock_guard<std::mutex> lg(mutex_);
        exit_ = true;
    }
    exit_cond_var_.notify_one();

    if (wait_until_stopped) {
        if (thread_.valid())
            thread_.wait();
    }
}

JHC_INLINE bool Thread::isRunning() const {
    return running_.load();
}

JHC_INLINE void Thread::run() {
    running_.store(true);

    SetCurrentThreadName(thread_name_.c_str());
    thread_id_ = Thread::GetCurThreadId();
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lg(mutex_);
            exit_cond_var_.wait(lg, [this]() { return (exit_ || !work_queue_.empty()); });
            if (exit_) {
                running_.store(false);
                return;
            }
            task = std::move(work_queue_.front());
            work_queue_.pop();
        }

        task();
    }
}

JHC_INLINE void Thread::SetCurrentThreadName(const char* name) {
#ifdef JHC_WIN
    struct {
        DWORD dwType;
        LPCSTR szName;
        DWORD dwThreadID;
        DWORD dwFlags;
    } threadname_info = {0x1000, name, static_cast<DWORD>(-1), 0};

    __try {
        ::RaiseException(0x406D1388, 0, sizeof(threadname_info) / sizeof(DWORD),
                         reinterpret_cast<ULONG_PTR*>(&threadname_info));
    } __except (EXCEPTION_EXECUTE_HANDLER) {  // NOLINT
    }
#elif defined JHC_LINUX
    prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(name));  // NOLINT
#endif
}

JHC_INLINE long Thread::GetCurThreadId() {
#ifdef JHC_WIN
    return GetCurrentThreadId();
#elif defined JHC_LINUX
    return static_cast<long>(syscall(__NR_gettid));
#endif
}
}  // namespace jhc
#endif