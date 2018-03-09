#include "pttdb.h"
#include "pttdb_internal.h"

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
create_comment(UUID main_id, char *poster, char *ip, int len, char *content, UUID comment_id)
{
    Err error_code = S_OK;

    time64_t create_milli_timestamp;

    Comment comment = {};
    bson_t comment_id_bson;
    bson_t comment_bson;

    error_code = get_milli_timestamp(&create_milli_timestamp);
    if (error_code) return error_code;

    error_code = gen_uuid_with_db(MONGO_COMMENT, comment_id);
    if (error_code) return error_code;

    // comment
    strncpy(comment.the_id, comment_id, sizeof(UUID));
    strncpy(comment.main_id, main_id, sizeof(UUID));
    comment.status = LIVE_STATUS_ALIVE;
    strcpy(comment.status_updater, poster);
    strcpy(comment.status_update_ip, ip);

    strcpy(comment.poster, poster);
    strcpy(comment.ip, ip);
    comment.create_milli_timestamp = create_milli_timestamp;
    strcpy(comment.updater, updater);
    strcpy(comment.update_ip, ip);
    comment.update_milli_timestamp = update_milli_timestamp;
    comment.len = len;
    strncpy(comment.buf, content, len);

    // db-comment
    bson_init(&comment_bson);
    error_code = _serialize_comment_bson(&comment, &comment_bson);
    if(error_code) {
        bson_destroy(&comment_bson);
        return error_code;
    }

    bson_init(&comment_id_bson);
    error_code = _serialize_uuid_bson(comment_id, MONGO_THE_ID, &comment_id_bson);
    if(error_code) {
        bson_destroy(&comment_bson);
        bson_destroy(&comment_id_bson);
        return error_code;
    }

    error_code = db_update_one(MONGO_COMMENT, &comment_id_bson, &comment_bson, true);
    bson_destroy(&comment_bson);
    bson_destroy(&comment_id_bson);
    if(error_code) {
        return error_code;
    }

    return S_OK;
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
_serialize_comment_bson(Comment *comment, bson_t *comment_bson)
{
    bool bson_status;

    bson_status = bson_append_int32(main_bson, "version", -1, comment->version);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_bin(comment_bson, "the_id", -1, comment->the_id, UUIDLEN);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_bin(comment_bson, "main_id", -1, comment->main_id, UUIDLEN);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_int32(comment_bson, "status", -1, comment->status);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_bin(comment_bson, "status_updater", -1, comment->status_updater, IDLEN);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_bin(comment_bson, "status_updater_ip", -1, comment->status_updater_ip, IPV4LEN);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_bin(comment_bson, "poster", -1, comment->poster, IDLEN);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_bin(comment_bson, "ip", -1, comment->ip, IPV4LEN);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_int64(comment_bson, "create_milli_timestamp", -1, comment->create_milli_timestamp);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_bin(comment_bson, "updater", -1, comment->updater, IDLEN);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_bin(comment_bson, "update_ip", -1, comment->update_ip, IPV4LEN);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_int64(comment_bson, "update_milli_timestamp", -1, comment->update_milli_timestamp);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_int32(comment_bson, "len", -1, comment->len);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_bin(comment_bson, "buf", -1, comment->buf, MAX_BUF_COMMENT);
    if (!bson_status) return S_ERR;

    return S_OK;
}
