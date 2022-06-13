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

#ifndef JHC_TIMER_HPP__
#define JHC_TIMER_HPP__
#pragma once

#include "jhc/config.hpp"
#include <functional>
#include <chrono>

namespace jhc {
// On Windows, the Timer has 10ms precision loss.
//
class Timer {
   public:
    using handler_t = std::function<void(std::size_t)>;

    Timer();
    ~Timer();

    /**
	 * Add a new timer.
	 *
	 * \param when The time at which the handler is invoked.
	 * \param handler The callable that is invoked when the timer fires.
	 * \param period The periodicity at which the timer fires. Only used for periodic timers.
	 */
    std::size_t add(
        const std::chrono::time_point<std::chrono::steady_clock>& when,
        handler_t&& handler,
        const std::chrono::microseconds& period = std::chrono::microseconds::zero());

    /**
	 * Overloaded `add` function that uses a `std::chrono::duration` instead of a
	 * `time_point` for the first timeout.
	 */
    template <class Rep, class Period>
    inline std::size_t add(const std::chrono::duration<Rep, Period>& when,
                           handler_t&& handler,
                           const std::chrono::microseconds& period = std::chrono::microseconds::zero()) {
        return add(std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::microseconds>(when),
                   std::move(handler),
                   period);
    }

    /**
	 * Overloaded `add` function that uses a uint64_t instead of a `time_point` for
	 * the first timeout and the period.
	 */
    std::size_t add(const uint64_t afterMicroseconds, handler_t&& handler, const uint64_t periodMicroseconds = 0);

    /**
	 * Removes the timer with the given id.
	 */
    bool remove(std::size_t id);

   protected:
    class Private;
    Private* p_ = nullptr;

   private:
    void run();
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/timer.cc"
#endif
#endif
