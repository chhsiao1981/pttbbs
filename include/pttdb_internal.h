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
 * UUID
 **********/
Err _serialize_uuid_bson(UUID uuid, const char *key, bson_t *uuid_bson);
Err _serialize_content_uuid_bson(UUID uuid, const char *key, int block_id, bson_t *uuid_bson);

/**********
 * Main
 **********/

/*
Err _split_main_contents(int fd_content, int len, UUID main_id, UUID content_id, int *n_line, int *n_block);
Err _split_main_contents_core(char *line, int bytes_in_line, UUID main_id, UUID content_id, MainContent *main_content_block, int *n_line, int *n_block);
Err _split_main_contents_init_main_content(MainContent *main_content_block, UUID main_id, UUID content_id, int *block_id);

Err _split_main_contents_save_main_content_block(MainContent *main_content_block);

Err _serialize_main_bson(MainHeader *main_header, bson_t *main_bson);
Err _deserialize_main_bson(bson_t *main_bson, MainHeader *main_header);
Err _serialize_update_main_bson(UUID content_id, char *updater, char *update_ip, time64_t update_milli_timestamp, int n_total_line, int n_total_block, int len_total, bson_t *main_bson);

Err _serialize_main_content_block_bson(MainContent *main_content_block, bson_t *main_content_block_bson);
Err _deserialize_main_content_block_bson(bson_t *main_content_block_bson, MainContent *main_content_block);
*/

/**********
 * ContentBlock
 **********/
Err _split_contents_core(char *buf, int bytes, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, int *n_line, int *n_block, bool is_deal_wtih_last_line_block);
Err _split_contents_core_core(char *line, int bytes_in_line, UUID ref_id, UUID content_id, ContentBlock *content, int *n_line, int *n_block);

Err _init_content_block(ContentBlock *content_block, UUID ref_id, UUID content_id, int *block_id);
Err _save_content_block(ContentBlock *content_block, enum MongoDBId mongo_db_id);
Err _split_contents_deal_with_last_line_block(int bytes_in_line, char *line, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, ContentBlock *content_block, int *n_line, int *n_block);

Err _serialize_content_block_bson(ContentBlock *content_block, bson_t *content_block_bson);
Err _deserialize_content_block_bson(bson_t *content_block_bson, ContentBlock *content_block);

/**********
 * Comment
 **********/
/*
Err _serialize_comment_bson(Comment *comment, bson_t *comment_bson);
Err _deserialize_comment_bson(bson_t *comment_bson, Comment *comment);
*/

/**********
 * Misc
 **********/

Err get_line_from_buf(char *p_buf, int offset_buf, int bytes, char *p_line, int offset_line, int *bytes_in_new_line);


#ifdef __cplusplus
}
#endif

#endif /* PTTDB_INTERNAL_H */