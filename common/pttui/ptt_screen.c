#include "cmpttui/ptt_screen.h"

PttScreen PTT_SCREEN = {};

Err
pttscreen_refresh_screen()
{
    int i = 0;
    Textline *p_line = PTT_SCREEN->top_line;

    Err error_code = pttscreen_set(0);
    if(error_code) return error_code;

    for(i = 0; i < n_line && p_line != NULL; i++, p_line = p_line->next) {
        error_code = pttscreen_printf(p_line->buf);
        if(error_code) break;

        error_code = pttscreen_move(1);        
        if(error_code) break;
    }
    if(error_code) return error_code;

    for(; i < n_line; i++) {
        error_code = pttscreen_printf("\n");
        if(error_code) break;
    }
    if(error_code) return error_code;

    error_code = pttscreen_set(0);

    return error_code;
}

Err
pttscreen_set(int pos)
{
    
}