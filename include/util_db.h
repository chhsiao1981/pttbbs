/* $Id$ */
#ifndef UTIL_DB_H
#define UTIL_DB_H

#include <mongoc.h>
#include "ptterr.h"

#ifdef __cplusplus
extern "C" {
#endif

// Mongo Post
#define MONGO_POST_DBNAME "post"

#define MONGO_MAIN_NAME "main"
#define MONGO_MAIN_CONTENT_NAME "main_content"

// Mongo Test
#define MONGO_TEST_DBNAME "test"

#define MONGO_TEST_NAME "test"

// Mongo the id
#define MONGO_THE_ID "the_id"
#define MONGO_BLOCK_ID "block_id"

enum {
    MONGO_MAIN,
    MONGO_MAIN_CONTENT,

    MONGO_TEST,

    N_MONGO_COLLECTIONS,
};

// initialization / free
Err init_mongo_global();
Err free_mongo_global();
Err init_mongo_collections();
Err free_mongo_collections();

// db-ops
Err db_set_if_not_exists(int collection, bson_t *key);
Err db_update_one(int collection, bson_t *key, bson_t *val, bool is_upsert);
Err db_find_one(int collection, bson_t *key, bson_t *fields, bson_t *result);

// bson-ops
#define bson_append_bin(b, key, key_length, binary, length) bson_append_binary(b, key, key_length, BSON_SUBTYPE_BINARY, length)

Err bson_exists(bson_t *b, char *name);
Err bson_get_value_int32(bson_t *b, char *name, int *value);
Err bson_get_value_bin(bson_t *b, char *name, char **value, int *len);
Err bson_get_value_bin_no_init(bson_t *b, char *name, int max_len, char *value, int *len);


#ifdef __cplusplus
}
#endif

#endif /* UTIL_DB_H */
