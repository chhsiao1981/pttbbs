#include "util_timestamp.h"

Err
milli_timestamp_to_year(time64_t milli_timestamp, int *year)
{
    struct tm tmp_tm = {};
    time64_t timestamp = 0;

    Err error_code = milli_timestamp_to_timestamp(milli_timestamp, &timestamp);
    localtime_r(&timestamp, &tmp_tm);
    *year = tmp_tm.tm_year + 1900;
    return S_OK;
}

Err
milli_timestamp_to_timestamp(time64_t milli_timestamp, time64_t *timestamp)
{
    *timestamp = milli_timestamp / 1000;
}

Err
datetime_to_timestamp(int year, int mm, int dd, int HH, int MM, int SS, time64_t *timestamp)
{
    char buf[MAX_BUF_TIMESTAMP_SIZE] = {};
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", year, mm, dd, HH, MM, SS);
    struct tm datetime = {};

    char *ret = strptime(buf, "%Y-%m-%d %H:%M:%S", &datetime);
    if(!ret) return S_ERR;

    time_t tmp_timestamp = mktime(&datetime);
    *timestamp = (time64_t) tmp_timestamp;

    return S_OK;
}
