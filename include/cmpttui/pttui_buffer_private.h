/* $Id$ */
#ifndef PTTUI_BUFFER_PRIVATE_H
#define PTTUI_BUFFER_PRIVATE_H

#include "ptterr.h"
#include "cmpttui/pttui_const.h"
#include "cmpttui/pttui_state.h"
#include "cmpttui/pttui_buffer.h"
#include "cmpttui/pttui_var.h"
#include "cmpttui/pttui_resource_info.h"
#include "cmpttui/pttui_resource_dict.h"
#include "cmpttui/pttui_thread_lock.h"
#include "cmpttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

/***
 * sync
 ***/
Err _sync_pttui_buffer_info_is_pre(PttUIState *state, PttUIBuffer *buffer, bool *is_pre);

Err _sync_pttui_buffer_info_get_buffer(PttUIState *state, PttUIBuffer *current_buffer, bool is_pre, PttUIBuffer **new_buffer);

/***
 * init buffer
 ***/
Err _pttui_buffer_init_buffer_no_buf_from_file_info(PttUIState *state, FileInfo *file_info, PttUIBuffer **buffer);

/***
 * extend 
 ***/
Err _extend_pttui_buffer(FileInfo *file_info, PttUIBuffer *head_buffer, PttUIBuffer *tail_buffer, PttUIBuffer *current_buffer, PttUIBuffer **new_head_buffer, PttUIBuffer **new_tail_buffer, int *n_new_buffer);

Err _extend_pttui_buffer_count_extra_pre_range(PttUIBuffer *buffer, int *n_extra_range);
Err _extend_pttui_buffer_count_extra_next_range(PttUIBuffer *buffer, int *n_extra_range);

Err _extend_pttui_buffer_extend_pre_buffer(FileInfo *file_info, PttUIBuffer *head_buffer, int n_buffer, PttUIBuffer **new_head_buffer, int *ret_n_buffer);
Err _extend_pttui_buffer_extend_pre_buffer_no_buf(PttUIBuffer *current_buffer, FileInfo *file_info, int n_buffer, PttUIBuffer **new_head_buffer, int *ret_n_buffer);
Err _extend_pttui_buffer_extend_pre_buffer_no_buf_core(PttUIBuffer *current_buffer, FileInfo *file_info, PttUIBuffer **new_buffer);
Err _extend_pttui_buffer_extend_pre_buffer_no_buf_main(PttUIBuffer *current_buffer, FileInfo *file_info, PttUIBuffer **new_buffer);
Err _extend_pttui_buffer_extend_pre_buffer_no_buf_comment(PttUIBuffer *current_buffer, FileInfo *file_info, PttUIBuffer **new_buffer);
Err _extend_pttui_buffer_extend_pre_buffer_no_buf_comment_reply(PttUIBuffer *current_buffer, FileInfo *file_info, PttUIBuffer **new_buffer);

Err _extend_pttui_buffer_extend_next_buffer(FileInfo *file_info, PttUIBuffer *tail_buffer, int n_buffer, PttUIBuffer **new_tail_buffer, int *ret_n_buffer);
Err _extend_pttui_buffer_extend_next_buffer_no_buf(PttUIBuffer *current_buffer, FileInfo *file_info, int n_buffer, PttUIBuffer **new_tail_buffer, int *ret_n_buffer);
Err _extend_pttui_buffer_extend_next_buffer_no_buf_core(PttUIBuffer *current_buffer, FileInfo *file_info, PttUIBuffer **new_buffer);
Err _extend_pttui_buffer_extend_next_buffer_no_buf_main(PttUIBuffer *current_buffer, FileInfo *file_info, PttUIBuffer **new_buffer);
Err _extend_pttui_buffer_extend_next_buffer_no_buf_comment(PttUIBuffer *current_buffer, FileInfo *file_info, PttUIBuffer **new_buffer);
Err _extend_pttui_buffer_extend_next_buffer_no_buf_comment_reply(PttUIBuffer *current_buffer, FileInfo *file_info, PttUIBuffer **new_buffer);

Err _pttui_buffer_info_to_resource_info(PttUIBuffer *head, PttUIBuffer *tail, PttUIResourceInfo *resource_info);
Err _pttui_buffer_info_set_buf_from_resource_dict(PttUIBuffer *head, PttUIBuffer *tail, PttUIResourceDict *resource_dict);
Err _pttui_buffer_info_set_buf_from_resource_dict_get_next_buf(char *p_buf, int buf_offset, int len, char **p_next_buf, int *buf_next_offset);

/***
 * shrink
 ***/
Err _sync_pttui_buffer_info_count_shrink_range(PttUIBufferInfo *buffer_info, int *n_shrink_range);

Err _sync_pttui_buffer_info_shrink_head(PttUIBufferInfo *buffer_info, int n_shrink_range);

Err _sync_pttui_buffer_info_shrink_tail(PttUIBufferInfo *buffer_info, int n_shrink_range);


#ifdef __cplusplus
}
#endif

#endif /* PTTUI_BUFFER_H */

