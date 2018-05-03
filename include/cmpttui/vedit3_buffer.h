/* $Id$ */
#ifndef VEDIT3_BUFFER_H
#define VEDIT3_BUFFER_H

#include "ptterr.h"
#include "cmpttui/vedit3_state.h"
#include "cmpttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SOFT_N_VEDIT3_BUFFER 100
#define HARD_N_VEDIT3_BUFFER 250
#define N_SHRINK_VEDIT3_BUFFER 600

typedef struct VEdit3Buffer {
    struct VEdit3Buffer *next;
    struct VEdit3Buffer *pre;

    UUID the_id;
    enum PttDBContentType content_type;    
    int block_offset;
    int line_offset;
    int comment_offset;
    
    enum StorageType storage_type;

    int len;
    int len_no_nl;
    char *buf;
} VEdit3Buffer;

typedef struct VEdit3BufferInfo {
    VEdit3Buffer *head;
    VEdit3Buffer *tail;
    int n_buffer;

    UUID main_id;
} VEdit3BufferInfo;

Err destroy_vedit3_buffer_info(VEdit3BufferInfo *buffer_info);

Err vedit3_buffer_split(VEdit3Buffer *buffer, int offset, int indent, VEdit3Buffer *new_buffer);

Err vedit3_buffer_is_begin_of_file(VEdit3Buffer *buffer, FileInfo *file_info, bool *is_begin);
Err vedit3_buffer_is_eof(VEdit3Buffer *buffer, FileInfo *file_info, bool *is_eof);

Err sync_vedit3_buffer_info(VEdit3BufferInfo *buffer_info, VEdit3Buffer *current_buffer, VEdit3State *state, FileInfo *file_info, VEdit3Buffer **new_buffer);

Err resync_all_vedit3_buffer_info(VEdit3BufferInfo *buffer_info, VEdit3State *state, FileInfo *file_info, VEdit3Buffer **new_buffer);

#ifdef __cplusplus
}
#endif

#endif /* VEDIT3_BUFFER_H */

