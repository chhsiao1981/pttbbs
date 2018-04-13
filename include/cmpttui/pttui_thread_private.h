/* $Id$ */
#ifndef PTTUI_THREAD_PRIVATE_H
#define PTTUI_THREAD_PRIVATE_H

#include "ptterr.h"
#include "cmpttui/pttui_thread_lock.h"
#include "cmpttui/vedit3.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <time.h>

#define NS_DEFAULT_SLEEP_THREAD 10000 // 10 micro-ts

Err _pttui_thread_is_end(bool *is_end);

#ifdef __cplusplus
}
#endif

#endif /* PTTUI_THREAD_PRIVATE_H */

