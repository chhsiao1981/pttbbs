/* $Id$ */
#ifndef PTTUI_RESOURCE_DICT_PRIVATE_H
#define PTTUI_RESOURCE_DICT_PRIVATE_H

#include "ptterr.h"
#include "cmpttui/pttui_buffer.h"
#include "cmpttui/pttui_thread_lock.h"
#include "cmpttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

Err _pttui_resource_dict_get_content_block_from_db_core(UUID uuid, int min_block_id, int max_block_id, enum MongoDBId mongo_db_id, PttUIResourceDict *resource_dict);

Err _pttui_resource_dict_get_content_block_from_db_core2(bson_t *q, int max_n_content_block, enum MongoDBId mongo_db_id, PttUIResourceDict *resource_dict);

Err _pttui_resource_dict_content_block_db_to_dict(bson_t **b_content_blocks, int n_content_block, PttUIResourceDict *resource_dict);

Err _pttui_resource_dict_comment_db_to_dict(bson_t **b_comments, int n_comment, PttUIResourceDict *resource_dict);

Err _pttui_resource_dict_add_data(UUID the_id, int block_id, int len, char *buf, PttUIResourceDict *resource_dict);

#ifdef __cplusplus
}
#endif

#endif /* PTTUI_RESOURCE_DICT_PRIVATE_H */

