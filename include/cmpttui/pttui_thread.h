/* $Id$ */
#ifndef PTTUI_THREAD_H
#define PTTUI_THREAD_H

#include "ptterr.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

enum PttUIThreadType {
    PTTUI_THREAD_DISP_BUFFER,
    N_PTTUI_THREAD_TYPE
};

enum PttUIThreadState {
    PTTUI_THREAD_STATE_START,
    PTTUI_THREAD_STATE_INIT_EDIT,
    PTTUI_THREAD_STATE_EDIT,
    PTTUI_THREAD_STATE_END
};

extern enum PttUIThreadState PTTUI_EXPECTED_STATE;
extern enum PttUIThreadState PTTUI_CURRENT_STATES[N_PTTUI_THREAD_TYPE];

extern pthread_t PTTUI_THREADS[N_PTTUI_THREAD_TYPE];
extern void *(*PTTUI_THREAD_FUNCS[N_PTTUI_THREAD_TYPE])(void *);

void *pttui_thread_disp_buffer(void *a);

Err pttui_thread_set_expected_state(enum PttUIThreadState thread_state);
Err pttui_thread_get_expected_state(enum PttUIThreadState *thread_state);
Err pttui_thread_set_current_state(enum PttUIThreadType thread_type, enum PttUIThreadState thread_state);
Err pttui_thread_get_current_state(enum PttUIThreadType thread_type, enum PttUIThreadState *thread_state);

Err init_pttui_thread();
Err destroy_pttui_thread();

Err pttui_thread_wait_buffer_loop(enum PttUIThreadState expected_state, int n_iter);

#ifdef __cplusplus
}
#endif

#endif /* PTTUI_THREAD_H */

