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

#ifndef AKALI_TIMEUTIL_HPP_
#define AKALI_TIMEUTIL_HPP_

#include "akali_hpp/arch.hpp"
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include <ctime>
#include <string>
#include <sstream>
#include <limits>

#ifdef AKALI_WIN
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

namespace akali_hpp {
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

    std::string toString(bool mill_precision = false,
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
        else if (mill_precision) {
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

    // 从1970-01-01 00:00:00到当前格林威治时间（UTC）所经过的微妙数
    static long long GetTimeStamp() {
#ifdef AKALI_WIN
        union {
            long long ns100;
            FILETIME ft;
        } fileTime;
        GetSystemTimeAsFileTime(&fileTime.ft);

        // lNowMicroMS中存储着从1970-01-01 00:00:00到当前格林威治时间（UTC）所经过的微妙数
        // 116444736000000000是从1601年1月1日00:00:00:000到1970年1月1日00:00:00:000所经过的100纳秒数
        long long lNowMicroMS = (long long)((fileTime.ns100 - 116444736000000000LL) / 10LL);

        return lNowMicroMS;
#else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        long long lNowMicroMS = tv.tv_sec * 1000000 + tv.tv_usec;
        return lNowMicroMS;
#endif
    }

    // Windows: precision is milliseconds
    static Time GetLocalTime() {
#ifdef AKALI_WIN
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
#ifdef AKALI_WIN
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

#ifdef AKALI_WIN
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
    long long UTCToTimeStamp(Time t) {
        struct tm tmUTC;
        tmUTC.tm_year = t.year - 1900;
        tmUTC.tm_mon = t.month - 1;
        tmUTC.tm_mday = t.day;
        tmUTC.tm_hour = t.hour;
        tmUTC.tm_min = t.minute;
        tmUTC.tm_sec = t.second;

        time_t timastampSec = mktime(&tmUTC);

        return (long long)timastampSec * 1000000;
    }
};

class TimerMeter {
   public:
    TimerMeter() { lStartTime_ = std::clock(); }

    void Restart() { lStartTime_ = std::clock(); }

    // ms
    long Elapsed() const { return std::clock() - lStartTime_; }

    long ElapsedMax() const { return (std::numeric_limits<std::clock_t>::max)() - lStartTime_; }

    long ElapsedMin() const { return 1L; }

   private:
    std::clock_t lStartTime_;
};
}  // namespace akali_hpp
#endif  // !AKALI_TIMEUTIL_HPP_
