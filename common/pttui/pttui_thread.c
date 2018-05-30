#include "cmpttui/pttui_thread.h"
#include "cmpttui/pttui_thread_private.h"

enum PttUIThreadState _PTTUI_THREAD_EXPECTED_STATE = PTTUI_THREAD_STATE_START;
enum PttUIThreadState _PTTUI_THREAD_BUFFER_STATE = PTTUI_THREAD_STATE_START;

pthread_t _PTTUI_THREAD_BUFFER;

Err (*_PTTUI_THREAD_BUFFER_FUNC_MAP[N_PTTUI_THREAD_STATE])() = {
    NULL,                      // start
    NULL,                      // init-read
    NULL,                      // read
    NULL                       // end
};

Err
InitPttUIThread()
{
    int ret = pthread_create(&_PTTUI_THREAD_BUFFER, NULL, PttUIThreadBuffer, NULL);
    if(ret) return S_ERR;

    return S_OK;
}

Err
DestroyPttUIThread()
{
    int ret = pthread_cancel(_PTTUI_THREAD_BUFFER);
    if(ret) return S_ERR;

    return S_OK;
}

void *
PttUIThreadBuffer(void *a GCC_UNUSED)
{
    Err error_code = S_OK;
    bool is_end = false;
    enum PttUIThreadState expected_state = PTTUI_THREAD_STATE_START;

    error_code = PttUIThreadSetBufferState(PTTUI_THREAD_STATE_START);
    if (error_code) return NULL;

    struct timespec req = {0, NS_DEFAULT_SLEEP_THREAD};
    struct timespec rem = {};
    int ret = 0;

    while(true) {
        error_code = _PttUIThreadIsEnd(&is_end);
        if(error_code) break;

        if(is_end) break;

        error_code = PttUIThreadGetExpectedState(&expected_state);
        if(error_code) break;

        error_code = PttUIThreadSetBufferState(expected_state);
        if(error_code) break;

        if(_PTTUI_THREAD_BUFFER_FUNC_MAP[expected_state]) {
            error_code = _PTTUI_THREAD_BUFFER_FUNC_MAP[expected_state]();
            if(error_code) break;
        }

        ret = nanosleep(&req, &rem);
        if(ret) break;
    }

    Err error_code_set_state = PttUIThreadSetBufferState(PTTUI_THREAD_STATE_END);
    if (!error_code && error_code_set_state) error_code = error_code_set_state;

    return NULL;
}

Err
PttUIThreadSetExpectedState(enum PttUIThreadState thread_state)
{
    // lock
    Err error_code = PttUIThreadLockWrlock(LOCK_PTTUI_THREAD_EXPECTED_STATE);
    if (error_code) return error_code;

    // do op
    _PTTUI_THREAD_EXPECTED_STATE = thread_state;

    // release lock
    error_code = PttUIThreadLockUnlock(LOCK_PTTUI_THREAD_EXPECTED_STATE);

    return error_code;
}

Err
PttUIThreadGetExpectedState(enum PttUIThreadState *thread_state)
{
    // lock
    Err error_code = PttUIThreadLockRdlock(LOCK_PTTUI_THREAD_EXPECTED_STATE);
    if (error_code) return error_code;

    // do op
    *thread_state = _PTTUI_THREAD_EXPECTED_STATE;

    // release lock
    error_code = PttUIThreadLockUnlock(LOCK_PTTUI_THREAD_EXPECTED_STATE);

    return error_code;
}

Err
PttUIThreadSetBufferState(enum PttUIThreadState thread_state)
{
    Err error_code = PttUIThreadLockWrlock(LOCK_PTTUI_THREAD_BUFFER_STATE);
    if (error_code) return error_code;

    _PTTUI_THREAD_BUFFER_STATE = thread_state;

    error_code = PttUIThreadLockUnlock(LOCK_PTTUI_THREAD_BUFFER_STATE);

    return error_code;
}

Err
PttUIThreadGetBufferState(enum PttUIThreadState *thread_state)
{
    Err error_code = PttUIThreadLockRdlock(LOCK_PTTUI_THREAD_BUFFER_STATE);
    if (error_code) return error_code;

    *thread_state = _PTTUI_THREAD_BUFFER_STATE;

    error_code = PttUIThreadLockUnlock(LOCK_PTTUI_THREAD_BUFFER_STATE);

    return error_code;
}

Err
PttUIThreadWaitBufferLoop(enum PttUIThreadState expected_state, int n_iter)
{
    Err error_code = S_OK;
    int ret_sleep = 0;
    enum PttUIThreadState current_state = PTTUI_THREAD_STATE_START;

    struct timespec req = {0, NS_DEFAULT_SLEEP_PTTUI_THREAD_WAIT_BUFFER_LOOP};
    struct timespec rem = {};

    int i = 0;
    for (i = 0; i < n_iter; i++) {
        error_code = PttUIThreadGetBufferState(&current_state);
        if (error_code) break;

        if (expected_state == current_state) break; // XXX maybe re-edit too quickly

        ret_sleep = nanosleep(&req, &rem);
        if (ret_sleep) {
            error_code = S_ERR;
            break;
        }
    }
    if (i == n_iter) error_code = S_ERR_BUSY;

    return error_code;
}

Err
_PttUIThreadIsEnd(bool *is_end)
{
    *is_end = false;
    return S_OK;
}
