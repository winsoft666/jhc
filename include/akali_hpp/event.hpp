/*******************************************************************************
 * Copyright (C) 2021 - 2026, winsoft666, <winsoft666@outlook.com>.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Expect bugs
 *
 * Please use and enjoy. Please let me know of any bugs/improvements
 * that you have found/implemented and I will fix/incorporate them into this
 * file.
 *******************************************************************************/
#ifndef AKALI_EVENT_HPP__
#define AKALI_EVENT_HPP__

#include <stdint.h>
#include <assert.h>
#include <mutex>

namespace akali_hpp {
class Event {
   public:
    Event(bool setted = false) :
        setted_(setted) {}

    ~Event() = default;

    void set() noexcept {
        std::unique_lock<std::mutex> ul(setted_mutex_);
        setted_ = true;
        setted_cond_var_.notify_all();
    }

    void unset() noexcept {
        std::unique_lock<std::mutex> ul(setted_mutex_);
        setted_ = false;
        setted_cond_var_.notify_all();
    }

    bool isSetted() noexcept {
        std::unique_lock<std::mutex> ul(setted_mutex_);
        return setted_;
    }

    bool wait(int32_t millseconds) noexcept {
        std::unique_lock<std::mutex> ul(setted_mutex_);
        setted_cond_var_.wait_for(ul, std::chrono::milliseconds(millseconds),
                                  [this] { return setted_; });
        return setted_;
    }

   protected:
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;
    bool setted_;
    std::mutex setted_mutex_;
    std::condition_variable setted_cond_var_;
};
}  // namespace akali_hpp
#endif  //!AKALI_EVENT_HPP__
