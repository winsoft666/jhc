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

#ifndef JHC_TIMEUTIL_HPP_
#define JHC_TIMEUTIL_HPP_
#pragma once

#include "jhc/config.hpp"
#include "jhc/arch.hpp"
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include <ctime>
#include <string>
#include <sstream>
#include <limits>

namespace jhc {
class Time {
   public:
    Time() {
        year = 0;
        month = 0;
        day = 0;
        minute = 0;
        second = 0;
        milliseconds = 0;
        microseconds = 0;
        nanoseconds = 0;
    }

    std::string toString(bool milli_precision = false,
                         bool micro_precision = false,
                         bool nano_precision = false) const;

   public:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int milliseconds;
    int microseconds;
    int nanoseconds;
};

class TimeUtil {
   public:
    static const int64_t kNumMillisecsPerSec = INT64_C(1000);
    static const int64_t kNumMicrosecsPerSec = INT64_C(1000000);
    static const int64_t kNumNanosecsPerSec = INT64_C(1000000000);

    static const int64_t kNumMicrosecsPerMillisec = kNumMicrosecsPerSec / kNumMillisecsPerSec;
    static const int64_t kNumNanosecsPerMillisec = kNumNanosecsPerSec / kNumMillisecsPerSec;
    static const int64_t kNumNanosecsPerMicrosec = kNumNanosecsPerSec / kNumMicrosecsPerSec;

    // The microseconds that since 1970-01-01 00:00:00(UTC)
    static int64_t GetCurrentTimestampByMicroSec();

    // The milliseconds that since 1970-01-01 00:00:00(UTC)
    static int64_t GetCurrentTimestampByMilliSec();

    // The seconds that since 1970-01-01 00:00:00(UTC)
    static int64_t GetCurrentTimestampBySec();

    // Windows: precision is milliseconds
    static Time GetLocalTime();

    // Windows: precision is milliseconds
    static Time GetUTCTime();

#ifdef JHC_WIN
    Time FILETIMEToUTC(unsigned int dwLowDateTime,
                       unsigned int dwHighDateTime);
#endif
    int64_t UTCToTimeStamp(Time t);
};

class TimeMeter {
   public:
    TimeMeter() { lStartTime_ = std::clock(); }

    void Restart() { lStartTime_ = std::clock(); }

    // ms
    long Elapsed() const { return std::clock() - lStartTime_; }

    long ElapsedMax() const { return (std::numeric_limits<std::clock_t>::max)() - lStartTime_; }

    long ElapsedMin() const { return 1L; }

   private:
    std::clock_t lStartTime_;
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/time_util.cc"
#endif
#endif  // !JHC_TIMEUTIL_HPP_
