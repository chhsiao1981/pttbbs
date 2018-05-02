/* $Id$ */
#ifndef PTTUI_UTIL_H
#define PTTUI_UTIL_H

#include "ptterr.h"
#include "pttui_const.h"
#include "cmpttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>
#include "vtkbd.h"

// XXX hack for DBCSAWARE in edit
#ifndef IS_BIG5_HI
#define IS_BIG5_HI(x) (0x81 <= (x) && (x) <= 0xfe)
#endif    
#ifndef IS_BIG5_LOS
#define IS_BIG5_LOS(x) (0x40 <= (x) && (x) <= 0x7e)
#endif    
#ifndef IS_BIG5_LOE
#define IS_BIG5_LOE(x) (0x80 <= (x) && (x) <= 0xfe)
#endif    
#ifndef IS_BIG5_LO
#define IS_BIG5_LO(x) (IS_BIG5_LOS(x) || IS_BIG5_LOE(x))
#endif
#ifndef IS_BIG5    
#define IS_BIG5(hi,lo) (IS_BIG5_HI(hi) && IS_BIG5_LO(lo))
#endif

enum PttUIFixCursorDir {
    PTTUI_FIX_CURSOR_DIR_RIGHT,
    PTTUI_FIX_CURSOR_DIR_LEFT,
};

Err pttui_raw_shift_right(char *s, int len);
Err pttui_raw_shift_left(char *s, int len);
Err pttui_ansi2n(int ansix, char *buf, int *nx);
Err pttui_n2ansi(int nx, char *buf, int *ansix);

inline Err pttui_phone_mode_switch(bool *is_phone, int *phone_mode);
Err pttui_phone_char(char c, int phone_mode, char **ret);
inline Err phone_mode_filter(char ch, int phone_mode, int last_phone_mode, char *ret);

Err pttui_is_eof(int line_no, FileInfo *file_info, bool is_full_comment_reply, bool *is_eof);

int pttui_mchar_len(unsigned char *str);

Err pttui_fix_cursor(char *str, int pos, enum PttUIFixCursorDir dir, int *new_pos);


#ifdef __cplusplus
}
#endif

#endif /* PTTUI_UTIL_H */

