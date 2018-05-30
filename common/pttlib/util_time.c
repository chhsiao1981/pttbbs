#define _XOPEN_SOURCE
#include "cmpttlib/util_time.h"
#include "cmpttlib/util_time_private.h"

char _CDATE_BUFFER[MAX_TIMESTAMP_BUF_SIZE] = {};

Err
GetMilliTimestamp(time64_t *milli_timestamp)
{
    struct timeval tv;

    int ret_code = gettimeofday(&tv, NULL);
    if (ret_code) return S_ERR;

    *milli_timestamp = ((time64_t)tv.tv_sec) * 1000L + ((time64_t)tv.tv_usec) / 1000L;

    return S_OK;
}

Err
MilliTimestampToYear(const time64_t milli_timestamp, int *year)
{
    struct tm tmp_tm = {};
    time64_t the_timestamp = 0;

    Err error_code = MilliTimestampToTimestamp(milli_timestamp, &the_timestamp);
    if(error_code) return error_code;

    the_timestamp += timezone - TZ_TAIPEI;

    localtime_r(&the_timestamp, &tmp_tm);
    *year = tmp_tm.tm_year + 1900;
    return S_OK;
}

time64_t MilliTimestampToTimestamp_ne(const time64_t milli_timestamp)
{
    return milli_timestamp / 1000;
}


Err
MilliTimestampToTimestamp(const time64_t milli_timestamp, time64_t *timestamp)
{
    *timestamp = milli_timestamp / 1000;

    return S_OK;
}

Err
DatetimeToTimestamp(const int year, const int mm, const int dd, const int HH, const int MM, const int SS, const int tz, time64_t *the_timestamp)
{
    char buf[MAX_TIMESTAMP_BUF_SIZE] = {};
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", year, mm, dd, HH, MM, SS);
    struct tm datetime = {};

    char *ret = strptime(buf, "%Y-%m-%d %H:%M:%S", &datetime);
    if(!ret) return S_ERR;

    time_t tmp_timestamp = mktime(&datetime) - timezone - tz;
    *the_timestamp = (time64_t) tmp_timestamp;

    return S_OK;
}

/**
 * 23+1 bytes, "12/31/2007 00:00:00 Mon\0"
 */
const char*
MilliTimestampToCdate_ne(const time64_t milli_timestamp)
{
    time_t    the_timestamp = MilliTimestampToTimestamp_ne(milli_timestamp);
    struct tm mytm = {};

    the_timestamp += timezone - TZ_TAIPEI;

    localtime_r(&the_timestamp, &mytm);
    strftime(_CDATE_BUFFER, sizeof(_CDATE_BUFFER), "%m/%d/%Y %T %a", &mytm);
    return _CDATE_BUFFER;
}

/**
 * 19+1 bytes, "12/31/2007 00:00:00\0"
 */
const char*
MilliTimestampToCdateLite_ne(const time64_t milli_timestamp)
{
    time_t    the_timestamp = MilliTimestampToTimestamp_ne(milli_timestamp);
    struct tm mytm = {};

    the_timestamp += timezone - TZ_TAIPEI;

    localtime_r(&the_timestamp, &mytm);
    strftime(_CDATE_BUFFER, sizeof(_CDATE_BUFFER), "%m/%d/%Y %T", &mytm);
    return _CDATE_BUFFER;

}

const char*
MilliTimestampToCdateDate_ne(const time64_t milli_timestamp)
{
    time_t    the_timestamp = MilliTimestampToTimestamp_ne(milli_timestamp);
    struct tm mytm = {};

    the_timestamp += timezone - TZ_TAIPEI;

    localtime_r(&the_timestamp, &mytm);
    strftime(_CDATE_BUFFER, sizeof(_CDATE_BUFFER), "%m/%d/%Y", &mytm);
    return _CDATE_BUFFER;
}

const char*
MilliTimestampToCdateMd_ne(const time64_t milli_timestamp)
{
    time_t    the_timestamp = MilliTimestampToTimestamp_ne(milli_timestamp);
    struct tm mytm = {};

    the_timestamp += timezone - TZ_TAIPEI;

    localtime_r(&the_timestamp, &mytm);
    strftime(_CDATE_BUFFER, sizeof(_CDATE_BUFFER), "%m/%d", &mytm);
    return _CDATE_BUFFER;
}

const char*
MilliTimestampToCdateMdHM_ne(const time64_t milli_timestamp)
{
    time_t    the_timestamp = MilliTimestampToTimestamp_ne(milli_timestamp);
    struct tm mytm = {};

    the_timestamp += timezone - TZ_TAIPEI;

    localtime_r(&the_timestamp, &mytm);
    strftime(_CDATE_BUFFER, sizeof(_CDATE_BUFFER), "%m/%d %H:%M", &mytm);
    return _CDATE_BUFFER;

}

const char*
MilliTimestampToCdateMdHMS_ne(const time64_t milli_timestamp)
{
    time_t    the_timestamp = MilliTimestampToTimestamp_ne(milli_timestamp);
    struct tm mytm = {};

    the_timestamp += timezone - TZ_TAIPEI;

    localtime_r(&the_timestamp, &mytm);
    strftime(_CDATE_BUFFER, sizeof(_CDATE_BUFFER), "%m/%d %H:%M:%S", &mytm);
    return _CDATE_BUFFER;
}

Err
AddTimespecWithNanosecs(struct timespec *a, int nanosecs)
{
    a->tv_nsec += nanosecs;
    if(a->tv_nsec < BILLION) return S_OK;

    a->tv_nsec -= BILLION;
    a->tv_sec++;

    return S_OK;
}
