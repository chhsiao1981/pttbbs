/* $Id$ */
#ifndef VEDIT3_PRIVATE_H
#define VEDIT3_PRIVATE_H

#include "ptterr.h"
#include "cmpttdb.h"
#include "cmmigrate_pttdb.h"
#include "cmpttui/pttui_thread.h"
#include "cmpttui/pttui_thread_lock.h"
#include "cmpttui/ptt_screen.h"
#include "cmpttui/vedit3_action.h"
#include "cmpttui/pttui_lua_bbs.h"
#include "cmpttui/pttui_util.h"
#include "proto_extra.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#define N_ITER_VEDIT3_WAIT_BUFFER_INIT 50
#define NS_DEFAULT_SLEEP_VEDIT3_WAIT_BUFFER_INIT 100000000 // 100 ms

#define N_ITER_VEDIT3_WAIT_BUFFER_THREAD_LOOP 100
#define NS_DEFAULT_SLEEP_VEDIT3_WAIT_BUFFER_THREAD_LOOP 100000000 // 100 ms

#define NS_DEFAULT_SLEEP_VEDIT3_REPL 10000000 // 10 ms

Err _vedit3_init_user();
Err _vedit3_init_editor(UUID main_id);
Err _vedit3_init_file_info(UUID main_id);

Err _vedit3_wait_buffer_init();

Err _vedit3_wait_buffer_thread_loop(enum PttUIThreadState expected_state);

Err _vedit3_set_buffer_current_state(VEdit3State *state);
Err _vedit3_get_buffer_current_state(VEdit3State *state);

// VEdit3 Misc
Err _vedit3_edit_msg();
Err _vedit3_init_dots();
Err _vedit3_loading();
Err _vedit3_loading_rotate_dots();

Err _vedit3_detect_attr(const char *ps, size_t len, int *p_attr);
int _vedit3_syn_lua_keyword(const char *text, int n, char *wlen);

// VEdit3 repl
Err _vedit3_repl_init();
Err _vedit3_repl(int *money);
Err _vedit3_check_healthy();
Err _vedit3_store_to_render();

// VEdit3 disp screen
Err _vedit3_disp_screen(int start_line, int end_line);

Err _vedit3_disp_line(int line, char *buf, int len);

Err _vedit3_edit_outs_attr_n(const char *text, int len, int attr);
Err _vedit3_edit_ansi_outs_n(const char *str, int n, int attr GCC_UNUSED);

#ifdef __cplusplus
}
#endif

#endif /* VEDIT3_PRIVATE_H */
