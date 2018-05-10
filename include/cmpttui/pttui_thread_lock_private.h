/* $Id$ */
#ifndef PTTUI_THREAD_LOCK_PRIVATE_H
#define PTTUI_THREAD_LOCK_PRIVATE_H

#include "ptterr.h"
#include <mongoc.h>        // XXX hack for bool

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <pthread.h>

#define NS_DEFAULT_SLEEP_LOCK 10000000 // 10ms
#define N_ITER_PTTUI_WRITE_LOCK 10 // write lock waits up to 100 ms
#define N_ITER_PTTUI_READ_LOCK 5 // read lock waits up to 50 ms

#ifdef __cplusplus
}
#endif

#endif /* PTTUI_THREAD_LOCK_PRIVATE_H */
