/* $Id$ */
#ifndef PTTUI_THREAD_PRIVATE_H
#define PTTUI_THREAD_PRIVATE_H

#include "pttconst.h"
#include "cmpttui/pttui_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

extern enum PttUIThreadState _PTTUI_THREAD_EXPECTED_STATE;
extern enum PttUIThreadState _PTTUI_THREAD_BUFFER_STATE;

extern pthread_t _PTTUI_THREAD_BUFFER;

Err _PttUIThreadIsEnd(bool *is_end);

#ifdef __cplusplus
}
#endif

#endif /* PTTUI_THREAD_PRIVATE_H */
