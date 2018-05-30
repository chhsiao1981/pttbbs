/* $Id$ */
#ifndef PTTUI_THREAD_LOCK_PRIVATE_H
#define PTTUI_THREAD_LOCK_PRIVATE_H

#include "pttconst.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

extern pthread_rwlock_t _PTTUI_RWLOCKS[N_PTTUI_THREAD_LOCK];

extern bool _PTTUI_WRITE_PRIORITY_LOCK[N_PTTUI_THREAD_LOCK];

#ifdef __cplusplus
}
#endif

#endif /* PTTUI_THREAD_LOCK_PRIVATE_H */
