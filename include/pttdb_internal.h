/* $ID$ */
#ifndef PTTDB_INTERNAL_H
#define PTTDB_INTERNAL_H

/**********
 * UUID
 **********/
Err _serialize_uuid_bson(UUID uuid, const char *key, bson_t *uuid_bson);
Err _serialize_content_uuid_bson(UUID uuid, const char *key, int block_id, bson_t *uuid_bson);

/**********
 * Mongo
 **********/
Err _db_set_if_not_exists(int collection, bson_t *key);
Err _db_update_one(int collection, bson_t *key, bson_t *val, bool is_upsert);

Err _bson_exists(bson_t *b, char *name);
Err _bson_get_value_int32(bson_t *b, char *name, int *value);

// XXX NEVER USE UNLESS IN TEST
Err _DB_FORCE_DROP_COLLECTION(int collection);

#endif /* PTTDB_INTERNAL_H */