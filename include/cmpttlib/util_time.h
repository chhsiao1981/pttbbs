/* $Id$ */
#ifndef UTIL_TIME_H
#define UTIL_TIME_H

#include "ptterr.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

// XXX hack for time64_t
typedef long time64_t;

#define TZ_TAIPEI 8

Err GetMilliTimestamp(time64_t *milli_timestamp);

Err MilliTimestampToYear(const time64_t milli_timestamp, int *year);

Err MilliTimestampToTimestamp(const time64_t milli_timestamp, time64_t *timestamp);

inline time64_t MilliTimestampToTimestamp_ne(const time64_t milli_timestamp);

Err DatetimeToTimestamp(const int year, const int mm, const int dd, const int HH, const int MM, const int SS, const int tz, time64_t *timestamp);

const char* MilliTimestampToCdate_ne(const time64_t milli_timestamp);

const char* MilliTimestampToCdateLite_ne(const time64_t milli_timestamp);

const char* MilliTimestampToCdateDate_ne(const time64_t milli_timestamp);

const char* MilliTimestampToCdateMd_ne(const time64_t milli_timestamp);

const char* MilliTimestampToCdateMdHM_ne(const time64_t milli_timestamp);

const char* MilliTimestampToCdateMdHMS_ne(const time64_t milli_timestamp);

Err AddTimespecWithNanosecs(struct timespec *a, int nanosecs);


#ifdef __cplusplus
}
#endif

#endif /* UTIL_TIME_H */
