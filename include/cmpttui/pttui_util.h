/* $Id$ */
#ifndef PTTUI_UTIL_H
#define PTTUI_UTIL_H

#include "ptterr.h"
#include "pttui_const.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>
#include "vtkbd.h"

Err pttui_raw_shift_right(char *s, int len);
Err pttui_raw_shift_left(char *s, int len);
Err pttui_ansi2n(int ansix, char *buf, int *nx);
Err pttui_n2ansi(int nx, char *buf, int *ansix);

inline Err pttui_phone_mode_switch(bool *is_phone, int *phone_mode);
Err pttui_phone_char(char c, int phone_mode, char *ret);
inline Err phone_mode_filter(char ch, int phone_mode, int last_phone_mode, char *ret);

Err pttui_is_eof(int line_no, FileInfo *file_info, bool is_full_comment_reply, bool *is_eof);


#ifdef __cplusplus
}
#endif

#endif /* PTTUI_UTIL_H */

