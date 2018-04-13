/* $Id$ */
#ifndef VEDIT3_BUFFER_PRIVATE_H
#define VEDIT3_BUFFER_PRIVATE_H

#include "ptterr.h"
#include "cmpttui/vedit3_state.h"
#include "cmpttui/vedit3_buffer.h"
#include "cmpttui/vedit3_resource_info.h"
#include "cmpttui/vedit3_resource_dict.h"
#include "cmpttui/pttui_thread_lock.h"
#include "cmpttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

Err _sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info(VEdit3State *state, FileInfo *file_info, VEdit3BufferInfo *buffer_info);

Err _construct_vedit3_buffer_from_vedit3_state_with_file_info(VEdit3State *state, FileInfo *file_info, VEdit3Buffer **buffer);

Err _sync_vedit3_buffer_info_is_pre(VEdit3State *state, VEdit3Buffer *buffer, bool *is_pre);

Err _sync_vedit3_buffer_info_get_buffer(VEdit3State *state, VEdit3Buffer *current_buffer, bool is_pre, VEdit3Buffer **new_buffer);

Err _sync_vedit3_buffer_info_count_extra_pre_range(VEdit3Buffer *buffer, int *n_extra_range);

Err _sync_vedit3_buffer_info_count_extra_next_range(VEdit3Buffer *buffer, int *n_extra_range);

Err _sync_vedit3_buffer_info_extend_pre_buffer(VEdit3BufferInfo *buffer_info, VEdit3State *state, FileInfo *file_info, int n_buffer);

Err _sync_vedit3_buffer_info_extend_pre_buffer_no_buf(VEdit3BufferInfo *buffer_info, FileInfo *file_info, int n_buffer);

Err _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_core(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer);

Err _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_main(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer);

Err _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer);

Err _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment_reply(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer);

Err _sync_vedit3_buffer_info_extend_next_buffer(VEdit3BufferInfo *buffer_info, VEdit3State *state, FileInfo *file_info, int n_buffer);

Err _sync_vedit3_buffer_info_extend_next_buffer_no_buf(VEdit3BufferInfo *buffer_info, FileInfo *file_info, int n_buffer);

Err _sync_vedit3_buffer_info_extend_next_buffer_no_buf_core(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer);

Err _sync_vedit3_buffer_info_extend_next_buffer_no_buf_main(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer);

Err _sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer);

Err _sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment_reply(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer);

Err _vedit3_buffer_info_to_resource_info(VEdit3Buffer *head, VEdit3ResourceInfo *resource_info);

Err _sync_vedit3_buffer_info_count_shrink_range(VEdit3BufferInfo *buffer_info, int *n_shrink_range);

Err _sync_vedit3_buffer_info_shrink_head(VEdit3BufferInfo *buffer_info, int n_shrink_range);

Err _sync_vedit3_buffer_info_shrink_tail(VEdit3BufferInfo *buffer_info, int n_shrink_range);


Err _vedit3_buffer_info_set_buf_from_resource_dict(VEdit3Buffer *head, VEdit3ResourceDict *resource_dict);

Err _vedit3_buffer_info_set_buf_from_resource_dict_get_next_buf(char *p_buf, int buf_offset, int len, char **p_next_buf, int *buf_next_offset);

#ifdef __cplusplus
}
#endif

#endif /* VEDIT3_BUFFER_H */

