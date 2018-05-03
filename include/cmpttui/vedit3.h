/* $Id$ */
#ifndef VEDIT3_H
#define VEDIT3_H

#include "ptterr.h"
#include "ptt_const.h"
#include "cmpttui/vedit3_state.h"
#include "cmpttui/vedit3_buffer.h"

#include "cmpttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <string.h>

#include "common.h"
#include "ansi.h"
#include "vtuikit.h"

#include "var.h"

enum VEdit3Attr {
    VEDIT3_ATTR_NORMAL   = 0x00,
    VEDIT3_ATTR_SELECTED = 0x01, // selected (reverse)
    VEDIT3_ATTR_MOVIECODE= 0x02, // pmore movie
    VEDIT3_ATTR_BBSLUA   = 0x04, // BBS Lua (header)
    VEDIT3_ATTR_COMMENT  = 0x08, // comment syntax
};

typedef struct VEdit3EditorStatus {
    bool is_insert;
    bool is_ansi;
    bool is_indent;
    bool is_phone;
    bool is_raw;
    bool is_mbcs;

    int phone_mode;

    int current_line;
    int current_col;
    int edit_margin;
    int last_margin;

    bool is_own_lock_buffer_info;
    bool is_redraw_everything;

    VEdit3Buffer *current_buffer;

    int mode0;
    int destuid0;
} VEdit3EditorStatus; 

extern VEdit3EditorStatus DEFAULT_VEDIT3_EDITOR_STATUS;
extern VEdit3EditorStatus VEDIT3_EDITOR_STATUS;

extern VEdit3State VEDIT3_STATE;
extern VEdit3State VEDIT3_BUFFER_STATE;

extern FileInfo VEDIT3_FILE_INFO;

extern VEdit3BufferInfo VEDIT3_BUFFER_INFO;
extern VEdit3Buffer *VEDIT3_DISP_TOP_LINE_BUFFER;
extern VEdit3Buffer *VEDIT3_DISP_END_LINE_BUFFER; // included

//extern VEdit3Buffer *VEDIT3_BUFFER_HEAD;

int vedit3_wrapper(const char *fpath, int saveheader, char title[TTLEN + 1], int flags, fileheader_t *fhdr, boardheader_t *bp);

Err vedit3(UUID main_id, char *title, int edflags, int *money);

Err vedit3_init_disp_buffer();

Err vedit3_disp_buffer();

Err vedit3_lock_buffer_info();

Err vedit3_unlock_buffer_info();

Err vedit3_wrlock_buffer_info();

Err vedit3_wrunlock_buffer_info();

Err vedit3_set_expected_state(UUID main_id, enum PttDBContentType top_line_content_type, UUID top_line_id, int top_line_block_offset, int top_line_line_offset, int top_line_comment_offset, int n_window_line);
Err vedit3_get_expected_state(VEdit3State *expected_state);


#ifdef __cplusplus
}
#endif

#endif /* VEDIT3_H */
