/* $Id$ */
#ifndef VEDIT3_ACTION_H
#define VEDIT3_ACTION_H

#include "ptterr.h"
#include "cmpttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

Err vedit3_action_to_store(bool *is_end);

Err vedit3_action_toggle_ansi();
Err vedit3_action_t_users();

Err vedit3_action_insert_char(int ch);

Err vedit3_action_move_left();

Err vedit3_action_move_right();

Err vedit3_action_move_up();
Err vedit3_action_move_down();

Err vedit3_action_move_pgup();
Err vedit3_action_move_pgdn();

Err vedit3_action_move_end_line();

Err vedit3_action_move_begin_line();

Err vedit3_action_redraw();

Err vedit3_action_show_help();

Err vedit3_action_delete_char();

Err vedit3_action_backspace();

Err vedit3_action_insert_new_line();

#ifdef __cplusplus
}
#endif

#endif /* VEDIT3_ACTION_H */

