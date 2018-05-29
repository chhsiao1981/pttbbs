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
InitPttuiThread()
{
    int ret = pthread_create(&_PTTUI_THREAD_BUFFER, NULL, PttUIThreadBuffer, NULL);
    if(ret) return S_ERR;

    return S_OK;
}

Err
DestroyPttuiThread()
{
    int ret = pthread_cancel(_PTTUI_THREAD_BUFFER);
    if(ret) return S_ERR;

    return S_OK;
}

void *
pttui_thread_buffer(void *a __attribute__ ((unused)))
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
    return S_OK;
}

Err
PttUIThreadGetExpectedState(enum PttUIThreadState *thread_state)
{
    return S_OK;
}

Err
PttUIThreadSetBufferState(enum PttUIThreadState thread_state)
{
    return S_OK;
}

Err
PttUIThreadGetBufferState(enum PttUIThreadState *thread_state)
{
    return S_OK;
}

Err
PttUIThreadWaitBufferLoop(enum PttUIThreadState expected_state, int n_iter)
{
    return S_OK;
}

Err
_PttUIThreadIsEnd(bool *is_end)
{
    *is_end = false;
    return S_OK;
}
