/* $Id$ */
#ifndef PTTDB_INTERNAL_H
#define PTTDB_INTERNAL_H

#include "ptterr.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>
#include <resolv.h>

/**********
 * Post
 **********/
Err _get_file_info_by_main_get_main_info(UUID main_id, FileInfo *file_info);
Err _get_file_info_by_main_get_content_block_info(FileInfo *file_info);

/**********
 * Main
 **********/

Err _serialize_main_bson(MainHeader *main_header, bson_t **main_bson);
Err _deserialize_main_bson(bson_t *main_bson, MainHeader *main_header);
Err _serialize_update_main_bson(UUID content_id, char *updater, char *update_ip, time64_t update_milli_timestamp, int n_total_line, int n_total_block, int len_total, bson_t **main_bson);

/**********
 * ContentBlock
 **********/
Err _split_contents_core(char *buf, int bytes, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, int *n_line, int *n_block, char *line, int *bytes_in_line, ContentBlock *content_block);

Err _split_contents_core_one_line(char *line, int bytes_in_line, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, ContentBlock *content_block, int *n_line, int *n_block);

Err _split_contents_deal_with_last_line_block(int bytes_in_line, char *line, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, ContentBlock *content_block, int *n_line, int *n_block);

Err _read_content_blocks_core(bson_t *key, int max_n_block, int block_id, enum MongoDBId mongo_db_id, ContentBlock *content_blocks, int *n_block, int *len);
Err _dynamic_read_content_blocks_core(bson_t *key, int max_n_block, int block_id, enum MongoDBId mongo_db_id, char *buf, int max_buf_size, ContentBlock *content_blocks, int *n_block, int *len);
Err _read_content_blocks_get_b_content_blocks(bson_t **b_content_blocks, bson_t *key, int max_n_block, int block_id, enum MongoDBId mongo_db_id, int *n_block);

Err _form_content_block_b_array_block_ids(int block_id, int max_n_block, bson_t **b);
Err _sort_b_content_blocks_by_block_id(bson_t **b_content_blocks, int n_block);
int _cmp_b_content_blocks_by_block_id(const void *a, const void *b);
Err _ensure_b_content_blocks_block_ids(bson_t **b_content_blocks, int start_block_id, int n_block);

Err _serialize_content_block_bson(ContentBlock *content_block, bson_t **content_block_bson);
Err _deserialize_content_block_bson(bson_t *content_block_bson, ContentBlock *content_block);

/**********
 * Comment
 **********/

Err _serialize_comment_bson(Comment *comment, bson_t **comment_bson);
Err _deserialize_comment_bson(bson_t *comment_bson, Comment *comment);
Err _get_comment_info_by_main_deal_with_result(bson_t *result, int n_result, int *n_total_comment, int *total_len);
Err _read_comments_get_b_comments(bson_t **b_comments, UUID main_id, time64_t create_milli_timestamp, char *poster, enum ReadCommentsOpType op_type, int max_n_comment, enum MongoDBId mongo_db_id, int *n_comment);
Err _read_comments_get_b_comments_same_create_milli_timestamp(bson_t **b_comments, UUID main_id, time64_t create_milli_timestamp, char *poster, enum ReadCommentsOpType op_type, int max_n_comment, enum MongoDBId mongo_db_id, int *n_comment);
Err _read_comments_get_b_comments_diff_create_milli_timestamp(bson_t **b_comments, UUID main_id, time64_t create_milli_timestamp, enum ReadCommentsOpType op_type, int max_n_comment, enum MongoDBId mongo_db_id, int *n_comment);
Err _read_comments_get_b_comments_core(bson_t **b_comments, bson_t *key, bson_t *sort, enum ReadCommentsOpType op_type, int max_n_comment, enum MongoDBId mongo_db_id, int *n_comment);
int _cmp_b_comments_ascending(const void *a, const void *b);
int _cmp_b_comments_descending(const void *a, const void *b);
Err _reverse_b_comments(bson_t **b_comments, int n_comment);

// for file-info
int _cmp_b_comments_by_comment_id(const void *a, const void *b);

// 
Err _dynamic_read_b_comment_comment_reply_by_ids_to_buf_core(bson_t **b_comments, int n_comment, bson_t *b_comment_content_dict, bson_t *b_comment_reply_dict, char *buf, int max_buf_size, int *n_read_comment, int *n_read_comment_reply, int *len_buf);


/**********
 * CommentReply
 **********/

Err _serialize_comment_reply_bson(CommentReply *comment_reply, bson_t **comment_reply_bson);
Err _deserialize_comment_reply_bson(bson_t *comment_reply_bson, CommentReply *comment_reply);
Err _deserialize_comment_reply_bson_with_buf(bson_t *comment_reply_bson, CommentReply *comment_reply);
Err _get_comment_reply_info_by_main_deal_with_result(bson_t *result, int n_result, int *n_comment_reply, int *n_line, int *total_len);

/**********
 * Misc
 **********/

Err get_line_from_buf(char *p_buf, int offset_buf, int bytes, char *p_line, int offset_line, int *bytes_in_new_line);
Err pttdb_count_lines(char *content, int len, int *n_line);

#ifdef __cplusplus
}
#endif

#endif /* PTTDB_INTERNAL_H */