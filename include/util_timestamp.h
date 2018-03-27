/* $Id$ */
#ifndef UTIL_TIMESTAMP_H
#define UTIL_TIMESTAMP_H

#include "ptterr.h"
#include "pttdb_uuid.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#define MAX_BUF_TIMESTAMP_SIZE 100

Err milli_timestamp_to_year(time64_t milli_timestamp, int *year);

Err milli_timestamp_to_timestamp(time64_t milli_timestamp, time64_t *timestamp);
Err datetime_to_timestamp(int year, int mm, int dd, int HH, int MM, int SS, time64_t *timestamp);

#ifdef __cplusplus
}
#endif

#endif /* UTIL_TIMESTAMP_H */