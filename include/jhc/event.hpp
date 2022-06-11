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

#ifndef JHC_EVENT_HPP__
#define JHC_EVENT_HPP__

#include <mutex>
#include <condition_variable>
#include "macros.hpp"

namespace jhc {
class Event {
   public:
    JHC_DISALLOW_COPY_MOVE(Event);
    Event(bool setted = false);

    ~Event() = default;

    void set() noexcept;

    void unset() noexcept;

    bool isSetted() noexcept;

    bool wait(int32_t millseconds) noexcept;

   protected:
    bool setted_;
    std::mutex setted_mutex_;
    std::condition_variable setted_cond_var_;
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/event.cc"
#endif

#endif  //!JHC_EVENT_HPP__
