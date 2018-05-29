/* $Id$ */
#ifndef UTIL_TIME_PRIVATE_H
#define UTIL_TIME_PRIVATE_H

#include "pttconst.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>
#include <stdio.h>
#include "config.h"

#ifndef MY_TZ
#define MY_TZ 8
#endif

#define BILLION 1000000000

#define MAX_TIMESTAMP_BUF_SIZE 24


#ifdef __cplusplus
}
#endif

#endif /* UTIL_TIME_PRIVATE_H */
