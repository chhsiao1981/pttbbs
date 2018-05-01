/* $Id$ */
#ifndef VEDIT3_ACTION_PRIVATE_H
#define VEDIT3_ACTION_PRIVATE_H

#include "ptterr.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
    
#include "vtkbd.h"

Err _vedit3_action_get_key(int *ch);
Err _vedit3_action_toggle_ansi();

#ifdef __cplusplus
}
#endif

#endif /* VEDIT3_ACTION_PRIVATE_H */

