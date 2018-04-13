/* $Id$ */
#ifndef VEDIT3_STATE_H
#define VEDIT3_STATE_H

#include "ptterr.h"
#include "cmpttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct VEdit3State {
    UUID main_id;
    enum PttDBContentType top_line_content_type;
    UUID top_line_id;
    int top_line_block_offset;
    int top_line_line_offset;
    int top_line_comment_offset;
    int n_window_line;
} VEdit3State;

#ifdef __cplusplus
}
#endif

#endif /* VEDIT3_STATE_H */
