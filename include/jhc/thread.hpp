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

#ifndef JHC_THREAD_HPP__
#define JHC_THREAD_HPP__

#include "jhc/arch.hpp"

#if defined JHC_WIN || defined JHC_LINUX
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif
#elif (defined JHC_LINUX)
#include <sys/prctl.h>
#include <unistd.h>
#include <sys/syscall.h>
#endif

#include "jhc/macros.hpp"

namespace jhc {
class Thread {
   public:
    JHC_DISALLOW_COPY_MOVE(Thread);

    Thread() :
        thread_id_(0), exit_(false) { running_.store(false); }

    Thread(const std::string& name) :
        thread_id_(0), exit_(false), thread_name_(name) {
        running_.store(false);
    }

    virtual ~Thread() { stop(true); }

    void setName(const std::string& name) { thread_name_ = name; }
    std::string name() const { return thread_name_; }

    long id() { return thread_id_; }

    bool start() {
        if (thread_.valid()) {
            if (thread_.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
                return false;
            }
        }
        thread_ = std::async(std::launch::async, &Thread::run, this);
        return true;
    }

    virtual void stop(bool wait_until_stopped) {
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

    bool isRunning() const { return running_.load(); }

    virtual void run() {
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

    template <class F, class... Args>
    auto invoke(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();

        {
            std::unique_lock<std::mutex> lock(mutex_);
            work_queue_.emplace([task]() { (*task)(); });
        }
        exit_cond_var_.notify_one();
        return res;
    }

    static void SetCurrentThreadName(const char* name) {
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

    static long GetCurThreadId() {
#ifdef JHC_WIN
        return GetCurrentThreadId();
#elif defined JHC_LINUX
        return static_cast<long>(syscall(__NR_gettid));
#endif
    }

   protected:
    std::string thread_name_;
    std::future<void> thread_;
    long thread_id_;
    std::mutex mutex_;
    std::condition_variable exit_cond_var_;
    bool exit_;
    std::queue<std::function<void()>> work_queue_;
    std::atomic_bool running_;
};
}  // namespace jhc
#endif
#endif  // !JHC_THREAD_HPP__
