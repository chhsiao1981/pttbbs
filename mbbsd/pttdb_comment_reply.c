#include "pttdb.h"
#include "pttdb_internal.h"

/**
 * @brief Create comment_reply from fd
 * @details Create comment_reply from fd
 *
 * @param title title
 * @param poster poster
 * @param ip ip
 * @param origin origin
 * @param web_link web_link
 * @param len length of the content
 * @param fd_content fd
 * @param main_id (to-compute)
 * @return Err
 */


/**
 * @brief Create comment_reply from fd
 * @details Create comment_reply from fd
 * 
 * @param main_id referring main_id
 * @param comment_id referring comment_id
 * @param poster [description]
 * @param ip [description]
 * @param len [description]
 * @param fd_content [description]
 * @param comment_reply_id [description]
 * @param comment_reply_content_id [description]
 */
Err
create_comment_reply_from_fd(UUID main_id, UUID comment_id, char *poster, char *ip, int len, int fd_content, UUID comment_reply_id, UUID comment_reply_content_id)
{

    Err error_code = S_OK;
    int n_line = 0;
    int n_block = 0;

    time64_t create_milli_timestamp;

    CommentReplyHeader comment_reply_header;

    error_code = get_milli_timestamp(&create_milli_timestamp);
    if (error_code) return error_code;

    error_code = gen_uuid_with_db(MONGO_COMMENT_REPLY, comment_reply_id);
    if (error_code) return error_code;

    error_code = gen_content_uuid_with_db(MONGO_COMMENT_REPLY_CONTENT, comment_reply_content_id);
    if (error_code) return error_code;

    // comment_reply_header
    strncpy(comment_reply_header.the_id, comment_reply_id, sizeof(UUID));
    strncpy(comment_reply_header.content_id, comment_reply_content_id, sizeof(UUID));
    strncpy(comment_reply_header.update_content_id, comment_reply_content_id, sizeof(UUID));

    strncpy(comment_reply_header.comment_id, comment_id, sizeof(UUID));
    strncpy(comment_reply_header.main_id, main_id, sizeof(UUID));

    comment_reply_header.status = LIVE_STATUS_ALIVE;

    strcpy(comment_reply_header.status_updater, poster);
    strcpy(comment_reply_header.status_update_ip, ip);

    strcpy(comment_reply_header.poster, poster);
    strcpy(comment_reply_header.ip, ip);
    strcpy(comment_reply_header.updater, poster);
    strcpy(comment_reply_header.update_ip, ip);

    main_header.create_milli_timestamp = create_milli_timestamp;
    main_header.update_milli_timestamp = create_milli_timestamp;

    // comment_reply_contents
    error_code = _split_comment_reply_contents(fd_content, len, comment_reply_id, comment_reply_content_id, &n_line, &n_block);
    if (error_code) {
        return error_code;
    }

    comment_reply_header.len_total = len;
    comment_reply_header.n_total_line = n_line;
    comment_reply_header.n_total_block = n_block;

    // db-comment-reply
    bson_t comment_reply_id_bson;
    bson_t comment_reply_bson;

    bson_init(&comment_reply_bson);
    bson_init(&comment_reply_id_bson);
    error_code = _serialize_comment_reply_bson(&comment_reply_header, &comment_reply_bson);
    if (error_code) {
        bson_destroy(&comment_reply_bson);
        bson_destroy(&comment_reply_id_bson);
        return error_code;
    }

    error_code = _serialize_uuid_bson(comment_reply_id, MONGO_THE_ID, &comment_reply_id_bson);
    if (error_code) {
        bson_destroy(&comment_reply_bson);
        bson_destroy(&comment_reply_id_bson);
        return error_code;
    }

    error_code = db_update_one(MONGO_COMMENT_REPLY, &comment_reply_id_bson, &comment_reply_bson, true);
    bson_destroy(&comment_reply_bson);
    bson_destroy(&comment_reply_id_bson);
    if (error_code) {
        return error_code;
    }

    return S_OK;
}
