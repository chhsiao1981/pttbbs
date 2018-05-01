/* $Id$ */
#ifndef VEDIT3_ACTION_PRIVATE_H
#define VEDIT3_ACTION_PRIVATE_H

#include "ptterr.h"
#include "cmpttui/vedit3.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
    
#include "vtkbd.h"
#include "proto.h"

Err _vedit3_action_toggle_ansi();
Err _vedit3_action_t_users();

Err _vedit3_action_get_key(int *ch);

Err _vedit3_action_ansi2n(int ansix, char *buf, int *nx);
Err _vedit3_action_n2ansi(int nx, char *buf, int *ansix);

#ifdef __cplusplus
}
#endif

#endif /* VEDIT3_ACTION_PRIVATE_H */

