/* $Id$ */
#ifndef PTTDB_FILE_INFO_PRIVATE_H
#define PTTDB_FILE_INFO_PRIVATE_H

#include "ptterr.h"
#include "cmpttdb/pttdb_main.h"
#include "cmpttdb/pttdb_content_block.h"
#include "cmpttdb/pttdb_comment.h"
#include "cmpttdb/pttdb_dict_idx_by_uu.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#define N_FILE_INFO_SET_COMMENT_INFO_BLOCK 1000

Err _get_file_info_set_content_block_info(UUID main_content_id, int n_content_block, FileInfo *file_info);

Err _get_file_info_set_comment_info(UUID main_id, FileInfo *file_info);

Err _get_file_info_set_comment_to_comment_info(bson_t **b_comments, int n_comment, CommentInfo *comments, int *n_read_comment);

Err _get_file_info_set_comment_replys_to_comment_info(bson_t **b_comments, int n_comment, CommentInfo *comments);

Err _file_info_get_next_line_main(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset);

Err _file_info_get_next_line_comment(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset);

Err _file_info_get_next_line_comment_reply(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset);

#ifdef __cplusplus
}
#endif

#endif /* PTTDB_FILE_INFO_PRIVATE_H */
