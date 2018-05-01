/* $Id$ */
#ifndef PTTUI_UTIL_H
#define PTTUI_UTIL_H

#include "ptterr.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>

Err pttui_raw_shift_right(char *s, int len);
Err pttui_raw_shift_left(char *s, int len);
Err pttui_ansi2n(int ansix, char *buf, int *nx);
Err pttui_n2ansi(int nx, char *buf, int *ansix);


#ifdef __cplusplus
}
#endif

#endif /* PTTUI_UTIL_H */

