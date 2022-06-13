#include "jhc/config.hpp"

#ifdef JHC_NOT_HEADER_ONLY
#include "../time_util.hpp"
#endif

#ifdef JHC_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif
#include <mmsystem.h>
#include <sys/timeb.h>
#pragma warning(disable : 4995)
#else
#include <sys/time.h>
#endif

namespace jhc {
JHC_INLINE std::string Time::toString(bool milli_precision,
                                      bool micro_precision,
                                      bool nano_precision) const {
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

// The microseconds that since 1970-01-01 00:00:00(UTC)
JHC_INLINE int64_t TimeUtil::GetCurrentTimestampByMicroSec() {
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
JHC_INLINE int64_t TimeUtil::GetCurrentTimestampByMilliSec() {
    return GetCurrentTimestampByMicroSec() / 1000;
}

// The seconds that since 1970-01-01 00:00:00(UTC)
JHC_INLINE int64_t TimeUtil::GetCurrentTimestampBySec() {
    return GetCurrentTimestampByMicroSec() / 1000000;
}

// Windows: precision is milliseconds
JHC_INLINE Time TimeUtil::GetLocalTime() {
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
JHC_INLINE Time TimeUtil::GetUTCTime() {
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
JHC_INLINE Time TimeUtil::FILETIMEToUTC(unsigned int dwLowDateTime,
                                        unsigned int dwHighDateTime) {
    FILETIME ft;
    ft.dwLowDateTime = dwLowDateTime;
    ft.dwHighDateTime = dwHighDateTime;

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
JHC_INLINE int64_t TimeUtil::UTCToTimeStamp(Time t) {
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
}  // namespace jhc