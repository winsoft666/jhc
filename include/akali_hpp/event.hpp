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
