/* $Id$ */
#ifndef VEDIT3_ACTION_PRIVATE_H
#define VEDIT3_ACTION_PRIVATE_H

#include "ptterr.h"
#include "cmpttui/vedit3.h"
#include "cmpttui/pttui_util.h"
#include "cmpttui/vedit3_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
    
#include "vtkbd.h"
#include "proto.h"

Err _vedit3_action_toggle_ansi();
Err _vedit3_action_t_users();

Err _vedit3_action_get_key(int *ch);


Err _vedit3_action_move_left();

Err _vedit3_action_move_right();

Err _vedit3_action_move_up();
Err _vedit3_action_move_up_ensure_top_of_window();

Err _vedit3_action_move_down();
Err _vedit3_action_move_down_ensure_end_of_window();

Err _vedit3_action_move_pgup();

Err _vedit3_action_move_end_line();

Err _vedit3_action_move_begin_line();


Err _vedit3_action_redraw();

Err _vedit3_action_show_help();


#ifdef __cplusplus
}
#endif

#endif /* VEDIT3_ACTION_PRIVATE_H */

