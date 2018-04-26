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
    int current_line;
    int current_col;
    int edit_margin;
    int last_margin;
    int n_modified_line;
    int *modified_line;

    int mode0;
    int destuid0;
} VEdit3EditorStatus; 

extern VEdit3EditorStatus VEDIT3_EDITOR_STATUS;

extern VEdit3State VEDIT3_STATE;
extern VEdit3State VEDIT3_BUFFER_STATE;

extern FileInfo VEDIT3_FILE_INFO;

extern VEdit3BufferInfo VEDIT3_BUFFER_INFO;
extern VEdit3Buffer *VEDIT3_DISP_TOP_LINE_BUFFER;

//extern VEdit3Buffer *VEDIT3_BUFFER_HEAD;

int vedit3_wrapper(const char *fpath, int saveheader, char title[TTLEN + 1], int flags, fileheader_t *fhdr, boardheader_t *bp);

Err vedit3(UUID main_id, char *title, int edflags, int *money);

Err vedit3_init_disp_buffer();

Err vedit3_disp_buffer();


#ifdef __cplusplus
}
#endif

#endif /* VEDIT3_H */
