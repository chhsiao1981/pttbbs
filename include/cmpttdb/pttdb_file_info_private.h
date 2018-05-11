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

Err _file_info_get_pre_line_main(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type);

Err _file_info_get_pre_line_comment(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type);

Err _file_info_get_pre_line_comment_reply(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type);

Err _file_info_get_next_line_main(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type);

Err _file_info_get_next_line_comment(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type);

Err _file_info_get_next_line_comment_reply(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type);

Err _save_file_info_to_db_main(FileInfo *file_info, char *user, char *ips);
Err _save_file_info_to_db_comment(FileInfo *file_info);
Err _save_file_info_to_db_comment_reply(FileInfo *file_info, char *user, char *ip);
Err _save_file_info_to_db_is_modified(ContentBlockInfo *content_blocks, int n_content_block, bool *is_modified);
Err _save_file_info_to_db_content_blocks_to_db(UUID main_id, UUID ref_id, UUID orig_id, enum PttDBContentType content_type, enum MongoDBId mongo_id, int n_content_block, ContentBlockInfo *content_blocks);

#ifdef __cplusplus
}
#endif

#endif /* PTTDB_FILE_INFO_PRIVATE_H */
