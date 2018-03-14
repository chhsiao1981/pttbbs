#include "pttdb.h"
#include "pttdb_internal.h"

enum Karma KARMA_BY_COMMENT_TYPE[] = {
    KARMA_GOOD,
    KARMA_BAD,
    KARMA_ARROW,
    0,
    0,                   // forward
    0,                   // other
};

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param main_id [description]
 * @param poster [description]
 * @param ip [description]
 * @param len [description]
 * @param content [description]
 * @param comment_id [description]
 */
Err
create_comment(UUID main_id, char *poster, char *ip, int len, char *content, enum CommentType comment_type, UUID comment_id)
{
    Err error_code = S_OK;

    time64_t create_milli_timestamp;

    Comment comment = {};

    error_code = get_milli_timestamp(&create_milli_timestamp);
    if (error_code) return error_code;

    error_code = gen_uuid_with_db(MONGO_COMMENT, comment_id);
    if (error_code) return error_code;

    // comment
    memcpy(comment.the_id, comment_id, sizeof(UUID));
    memcpy(comment.main_id, main_id, sizeof(UUID));
    comment.status = LIVE_STATUS_ALIVE;
    strcpy(comment.status_updater, poster);
    strcpy(comment.status_update_ip, ip);

    comment.comment_type = comment_type;
    comment.karma = KARMA_BY_COMMENT_TYPE[comment_type];

    strcpy(comment.poster, poster);
    strcpy(comment.ip, ip);
    comment.create_milli_timestamp = create_milli_timestamp;
    strcpy(comment.updater, poster);
    strcpy(comment.update_ip, ip);
    comment.update_milli_timestamp = create_milli_timestamp;
    comment.len = len;
    memcpy(comment.buf, content, len);

    // db-comment
    bson_t *comment_id_bson = NULL;
    bson_t *comment_bson = NULL;

    error_code = _serialize_comment_bson(&comment, &comment_bson);
    if (!error_code) {
        error_code = _serialize_uuid_bson(comment_id, &comment_id_bson);
    }

    if (!error_code) {
        error_code = db_update_one(MONGO_COMMENT, comment_id_bson, comment_bson, true);
    }

    bson_safe_destroy(&comment_bson);
    bson_safe_destroy(&comment_id_bson);

    return error_code;
}

Err
read_comment(UUID comment_id, Comment *comment)
{
    Err error_code = S_OK;
    bson_t *key = BCON_NEW(
                      "the_id", BCON_BINARY(comment_id, UUIDLEN)
                  );

    bson_t *db_result = NULL;
    error_code = db_find_one(MONGO_COMMENT, key, NULL, &db_result);

    if (!error_code) {
        error_code = _deserialize_comment_bson(db_result, comment);
    }

    bson_safe_destroy(&key);
    bson_safe_destroy(&db_result);
    return error_code;
}

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param main_id [description]
 * @param updater [description]
 * @param char [description]
 * @return [description]
 */
Err
delete_comment(UUID comment_id, char *updater, char *ip) {
    Err error_code = S_OK;

    bson_t *key = BCON_NEW(
                      "the_id", BCON_BINARY(comment_id, UUIDLEN)
                  );

    bson_t *val = BCON_NEW(
                      "status_updater", BCON_BINARY((unsigned char *)updater, IDLEN),
                      "status", BCON_INT32(LIVE_STATUS_DELETED),
                      "status_update_ip", BCON_BINARY((unsigned char *)ip, IPV4LEN)
                  );

    error_code = db_update_one(MONGO_COMMENT, key, val, true);

    bson_safe_destroy(&key);
    bson_safe_destroy(&val);

    return error_code;
}

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param comment [description]
 * @param comment_bson [description]
 *
 * @return [description]
 */
Err
_serialize_comment_bson(Comment *comment, bson_t **comment_bson)
{
    *comment_bson = BCON_NEW(
                        "version", BCON_INT32(comment->version),
                        "the_id", BCON_BINARY(comment->the_id, UUIDLEN),
                        "main_id", BCON_BINARY(comment->main_id, UUIDLEN),
                        "status", BCON_INT32(comment->status),
                        "status_updater", BCON_BINARY((unsigned char *)comment->status_updater, IDLEN),
                        "status_update_ip", BCON_BINARY((unsigned char *)comment->status_update_ip, IPV4LEN),
                        "comment_type", BCON_INT32(comment->comment_type),
                        "karma", BCON_INT32(comment->karma),
                        "poster", BCON_BINARY((unsigned char *)comment->poster, IDLEN),
                        "ip", BCON_BINARY((unsigned char *)comment->ip, IPV4LEN),
                        "create_milli_timestamp", BCON_INT64(comment->create_milli_timestamp),
                        "updater", BCON_BINARY((unsigned char *)comment->updater, IDLEN),
                        "update_ip", BCON_BINARY((unsigned char *)comment->update_ip, IPV4LEN),
                        "update_milli_timestamp", BCON_INT64(comment->update_milli_timestamp),
                        "len", BCON_INT32(comment->len),
                        "buf", BCON_BINARY((unsigned char *)comment->buf, comment->len)
                    );

    return S_OK;
}

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param comment_bson [description]
 * @param comment [description]
 *
 * @return [description]
 */
Err
_deserialize_comment_bson(bson_t *comment_bson, Comment *comment)
{
    Err error_code;

    int len;
    error_code = bson_get_value_int32(comment_bson, "version", &comment->version);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "the_id", UUIDLEN, comment->the_id, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "main_id", UUIDLEN, comment->main_id, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(comment_bson, "status", &comment->status);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(comment_bson, "comment_type", &comment->comment_type);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(comment_bson, "karma", &comment->karma);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "status_updater", IDLEN, &comment->status_updater, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "status_update_ip", IPV4LEN, &comment->status_update_ip, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "poster", IDLEN, &comment->poster, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "ip", IPV4LEN, &comment->ip, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_int64(comment_bson, "create_milli_timestamp", &comment->create_milli_timestamp);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "updater", IDLEN, &comment->updater, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "update_ip", IPV4LEN, &comment->update_ip, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_int64(comment_bson, "update_milli_timestamp", &comment->update_milli_timestamp);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(comment_bson, "len", &comment->len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "buf", MAX_BUF_COMMENT, comment->buf, &len);
    if (error_code) return error_code;
}
