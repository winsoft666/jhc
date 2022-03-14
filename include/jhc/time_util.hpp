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

#include "jhc/arch.hpp"
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include <ctime>
#include <string>
#include <sstream>
#include <limits>

#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif
#include <mmsystem.h>
#include <sys/timeb.h>
#pragma warning(disable : 4995)
#else
#include <sys/time.h>
#endif

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
                         bool nano_precision = false) const {
        char szString[512];
        if (nano_precision) {
            snprintf(szString, 512, "%04d/%02d/%02d %02u:%02u:%02u:%03u:%03u:%03u", year, month, day,
                     hour, minute, second, milliseconds, microseconds, nanoseconds);
        }
        else if (micro_precision) {
            snprintf(szString, 512, "%04d/%02d/%02d %02u:%02u:%02u:%03u:%03u", year, month, day, hour,
                     minute, second, milliseconds, microseconds);
        }
        else if (milli_precision) {
            snprintf(szString, 512, "%04d/%02d/%02d %02u:%02u:%02u:%03u", year, month, day, hour, minute,
                     second, milliseconds);
        }
        else {
            snprintf(szString, 512, "%04d/%02d/%02d %02u:%02u:%02u", year, month, day, hour, minute,
                     second);
        }

        return szString;
    }

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
    static int64_t GetCurrentTimestampByMicroSec() {
#ifdef JHC_WIN
        union {
            int64_t ns100;
            FILETIME ft;
        } fileTime;
        GetSystemTimeAsFileTime(&fileTime.ft);

        // 116444736000000000 is the number of total 100 nanoseconds that from 1601/1/1 00:00:00:000 to 1970/1/1 00:00:00:000
        int64_t lNowMicroMS = (long long)((fileTime.ns100 - 116444736000000000LL) / 10LL);

        return lNowMicroMS;
#else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int64_t lNowMicroMS = tv.tv_sec * 1000000 + tv.tv_usec;
        return lNowMicroMS;
#endif
    }

    // The milliseconds that since 1970-01-01 00:00:00(UTC)
    static int64_t GetCurrentTimestampByMilliSec() {
        return GetCurrentTimestampByMicroSec() / 1000;
    }

    // The seconds that since 1970-01-01 00:00:00(UTC)
    static int64_t GetCurrentTimestampBySec() {
        return GetCurrentTimestampByMicroSec() / 1000000;
    }

    // Windows: precision is milliseconds
    static Time GetLocalTime() {
#ifdef JHC_WIN
        Time t;
        SYSTEMTIME st;
        ::GetLocalTime(&st);

        t.year = st.wYear;
        t.month = st.wMonth;
        t.day = st.wDay;
        t.hour = st.wHour;
        t.minute = st.wMinute;
        t.second = st.wSecond;
        t.milliseconds = st.wMilliseconds;

        return t;
#else
        Time t;
        struct timespec ts = {0, 0};
        struct tm tm = {};
        if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
            time_t tim = ts.tv_nsec;
            if (localtime_r(&tim, &tm)) {
                t.year = tm.tm_year;
                t.month = tm.tm_mon;
                t.day = tm.tm_mday;
                t.hour = tm.tm_hour;
                t.minute = tm.tm_min;
                t.second = tm.tm_sec;
            }
        }

        return t;
#endif
    }

    // Windows: precision is milliseconds
    static Time GetUTCTime() {
#ifdef JHC_WIN
        Time t;
        SYSTEMTIME st;
        GetSystemTime(&st);

        t.year = st.wYear;
        t.month = st.wMonth;
        t.day = st.wDay;
        t.hour = st.wHour;
        t.minute = st.wMinute;
        t.second = st.wSecond;
        t.milliseconds = st.wMilliseconds;

        return t;

#else
        Time t;
        struct timespec ts = {0, 0};
        struct tm tm = {};
        if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
            time_t tim = ts.tv_nsec;
            if (gmtime_r(&tim, &tm)) {
                t.year = tm.tm_year;
                t.month = tm.tm_mon;
                t.day = tm.tm_mday;
                t.hour = tm.tm_hour;
                t.minute = tm.tm_min;
                t.second = tm.tm_sec;
            }
        }

        return t;
#endif
    }

#ifdef JHC_WIN
    Time FILETIMEToUTC(FILETIME ft) {
        SYSTEMTIME st;
        Time t;
        if (FileTimeToSystemTime(&ft, &st)) {
            t.year = st.wYear;
            t.month = st.wMonth;
            t.day = st.wDay;
            t.hour = st.wHour;
            t.minute = st.wMinute;
            t.second = st.wSecond;
            t.milliseconds = st.wMilliseconds;
        }

        return t;
    }
#endif
    int64_t UTCToTimeStamp(Time t) {
        struct tm tmUTC;
        tmUTC.tm_year = t.year - 1900;
        tmUTC.tm_mon = t.month - 1;
        tmUTC.tm_mday = t.day;
        tmUTC.tm_hour = t.hour;
        tmUTC.tm_min = t.minute;
        tmUTC.tm_sec = t.second;

        time_t timastampSec = mktime(&tmUTC);

        return (int64_t)timastampSec * 1000000;
    }
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
#endif  // !JHC_TIMEUTIL_HPP_
