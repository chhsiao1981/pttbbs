/* $Id$ */
#ifndef VEDIT3_ACTION_PRIVATE_H
#define VEDIT3_ACTION_PRIVATE_H

#include "ptterr.h"
#include "cmpttui/vedit3.h"
#include "cmpttui/pttui_util.h"
#include "cmpttui/pttui_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
    
#include "vtkbd.h"
#include "proto.h"

Err _vedit3_action_toggle_ansi();
Err _vedit3_action_t_users();

Err _vedit3_action_get_key(int *ch);

Err _vedit3_action_insert_ch(int ch);

Err _vedit3_action_move_left();

Err _vedit3_action_move_right();

Err _vedit3_action_move_up();
Err _vedit3_action_move_up_ensure_top_of_window();

Err _vedit3_action_move_down();
Err _vedit3_action_move_down_ensure_end_of_window();

Err _vedit3_action_move_pgup();
Err _vedit3_action_move_pgdn();

Err _vedit3_action_move_end_line();

Err _vedit3_action_move_begin_line();

Err _vedit3_action_redraw();

Err _vedit3_action_show_help();

Err _vedit3_action_to_store_main(int ch, bool *is_end);
Err _vedit3_action_to_store_comment(int ch, bool *is_end);
Err _vedit3_action_to_store_comment_reply(int ch, bool *is_end);

Err _vedit3_action_insert_dchar(const char *dchar);
Err _vedit3_action_insert_char(int ch);
Err _vedit3_action_ensure_buffer_wrap();
Err _vedit3_action_buffer_split(PttUIBuffer *current_buffer, int pos, int indent, PttUIBuffer **new_buffer);

Err _vedit3_action_ensure_current_col(int current_col);

Err _vedit3_action_delete_char();
Err _vedit3_action_delete_char_core();
Err _vedit3_action_backspace();
Err _vedit3_action_concat_next_line();
Err _vedit3_action_delete_line(PttUIBuffer **buffer);
Err _vedit3_action_insert_new_line();

#ifdef __cplusplus
}
#endif

#endif /* VEDIT3_ACTION_PRIVATE_H */

