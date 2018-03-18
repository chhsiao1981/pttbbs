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

char *_read_comments_op_type[] = {
    "$lt",
    "$lte",
    "$gt",
    "$gte",
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

    // use associate to associate content directly
    Comment comment = {};
    associate_comment(&comment, content, len);
    comment.len = len;

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
    dissociate_comment(&comment);

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
        "status", BCON_INT32((int)LIVE_STATUS_DELETED),
        "status_update_ip", BCON_BINARY((unsigned char *)ip, IPV4LEN)
    );

    error_code = db_update_one(MONGO_COMMENT, key, val, true);

    bson_safe_destroy(&key);
    bson_safe_destroy(&val);

    return error_code;
}

Err
get_comment_info_by_main(UUID main_id, int *n_total_comment, int *total_len)
{
    Err error_code = S_OK;
    bson_t *pipeline = BCON_NEW(
        "pipeline", "[",
            "{",
                "$match", "{",
                    "main_id", BCON_BINARY(main_id, UUIDLEN),
                    "status", BCON_INT32((int)LIVE_STATUS_ALIVE),
                "}",
            "}",
            "{",
                "$group", "{",
                    "_id", BCON_NULL,
                    "count", "{",
                        "$sum", BCON_INT32(1),
                    "}",
                    "len", "{",
                        "$sum", "$len",
                    "}",
                "}",
            "}",
        "]"
    );

    if (pipeline == NULL) error_code = S_ERR;

    bson_t *result = NULL;

    int n_result = 0;
    if (!error_code) {
        error_code = db_aggregate(MONGO_COMMENT, pipeline, 1, &result, &n_result);
    }

    if (!error_code) {
        error_code = _get_comment_info_by_main_deal_with_result(result, n_result, n_total_comment, total_len);
    }

    bson_safe_destroy(&result);
    bson_safe_destroy(&pipeline);

    return error_code;
}

Err
get_comment_count_by_main(UUID main_id, int *count)
{
    Err error_code = S_OK;
    bson_t *key = BCON_NEW(
        "main_id", BCON_BINARY(main_id, UUIDLEN),
        "status", BCON_INT32((int)LIVE_STATUS_ALIVE)
    );

    error_code = db_count(MONGO_COMMENT, key, count);

    bson_safe_destroy(&key);

    return error_code;
}

Err
init_comment_buf(Comment *comment)
{
    if (comment->buf != NULL) return S_OK;

    comment->buf = malloc(MAX_BUF_COMMENT);
    if (comment->buf == NULL) return S_ERR;

    comment->max_buf_len = MAX_BUF_COMMENT;
    bzero(comment->buf, MAX_BUF_COMMENT);
    comment->len = 0;

    return S_OK;
}

Err
destroy_comment(Comment *comment)
{
    if (comment->buf == NULL) return S_OK;

    free(comment->buf);
    comment->buf = NULL;
    comment->max_buf_len = 0;
    comment->len = 0;

    return S_OK;
}

Err
associate_comment(Comment *comment, char *buf, int max_buf_len)
{
    if (comment->buf != NULL) return S_ERR;

    comment->buf = buf;
    comment->max_buf_len = max_buf_len;
    comment->len = 0;

    return S_OK;
}

Err
dissociate_comment(Comment *comment)
{
    if (comment->buf == NULL) return S_OK;

    comment->buf = NULL;
    comment->max_buf_len = 0;
    comment->len = 0;

    return S_OK;
}

Err
read_comments_by_main(UUID main_id, time64_t create_milli_timestamp, char *poster, enum ReadCommentsOpType op_type, int max_n_comment, enum MongoDBId mongo_db_id, Comment *comments, int *n_comment, int *len)
{

    Err error_code = S_OK;

    // init db-results
    bson_t **b_comments = malloc(sizeof(bson_t *) * max_n_comment);
    if (b_comments == NULL) return S_ERR_INIT;
    bzero(b_comments, sizeof(bson_t *) * max_n_comment);

    error_code = _read_comments_get_b_comments(b_comments, main_id, create_milli_timestamp, poster, op_type, max_n_comment, mongo_db_id, n_comment);

    int tmp_n_comment = *n_comment;
    bson_t **p_b_comments = b_comments;
    Comment *p_comments = comments;

    int tmp_len = 0;
    if (!error_code) {
        for (int i = 0; i < tmp_n_comment; i++) {
            error_code = _deserialize_comment_bson(*p_b_comments, p_comments);

            tmp_len += p_comments->len;
            p_b_comments++;
            p_comments++;

            if (error_code) {
                *n_comment = i;
                break;
            }
        }
    }

    *len = tmp_len;

    // free
    safe_free_b_list(&b_comments, tmp_n_comment);

    return error_code;
}

Err
update_comment_reply_to_comment(UUID comment_id, UUID comment_reply_id, int n_comment_reply_line)
{
    Err error_code = S_OK;
    bson_t *key = BCON_NEW(
        "the_id", BCON_BINARY(comment_id, UUIDLEN)
        );

    bson_t *val = BCON_NEW(
        "comment_reply_id", BCON_BINARY(comment_reply_id, UUIDLEN),
        "n_comment_reply_line", BCON_INT32(n_comment_reply_line)
        );

    error_code = db_update_one(MONGO_COMMENT, key, val, false);

    bson_safe_destroy(&key);
    bson_safe_destroy(&val);

    return error_code;
}

Err
get_newest_comment(UUID main_id, UUID comment_id, time64_t *create_milli_timestamp, char *poster, int *n_comment)
{
    Err error_code = S_OK;
    bson_t *key = BCON_NEW(
        "main_id", BCON_BINARY(main_id, UUIDLEN),
        "status", BCON_INT32((int)LIVE_STATUS_ALIVE),
        "create_milli_timestamp", "{",
            "$lt", BCON_INT64(MAX_CREATE_MILLI_TIMESTAMP),
        "}"
        );

    bson_t *fields = BCON_NEW(
        "_id", BCON_BOOL(false),
        "the_id", BCON_BOOL(true),
        "create_milli_timestamp", BCON_BOOL(true),
        "poster", BCON_BOOL(true)
        );

    bson_t *sort = BCON_NEW(
        "create_milli_timestamp", BCON_INT32(-1),
        "poster", BCON_INT32(-1)
        );

    bson_t *result = NULL;
    int len = 0;
    error_code = db_find(MONGO_COMMENT, key, fields, sort, 1, &len, &result);
    if(!error_code && !len) {
        error_code = S_ERR_NOT_EXISTS;
    }
    if(!error_code) {
        error_code = bson_get_value_bin(result, "the_id", UUIDLEN, (char *)comment_id, &len);
    }
    if(!error_code) {
        error_code = bson_get_value_int64(result, "create_milli_timestamp", (long *)create_milli_timestamp);
    }
    if(!error_code) {
        error_code = bson_get_value_bin(result, "poster", IDLEN, poster, &len);
    }

    int count_eq_create_milli_timestamp = 0;

    bson_t *count_query_eq = BCON_NEW(
        "main_id", BCON_BINARY(main_id, UUIDLEN),
        "status", BCON_INT32((int)LIVE_STATUS_ALIVE),
        "create_milli_timestamp", BCON_INT64(*create_milli_timestamp),
        "poster", "{",
            "$lte", BCON_BINARY((unsigned char *)poster, IDLEN),
        "}"
        );

    if(!error_code) {
        error_code = db_count(MONGO_COMMENT, count_query_eq, &count_eq_create_milli_timestamp);
    }

    int count_lt_create_milli_timestamp = 0;
    bson_t *count_query_lt = BCON_NEW(
        "main_id", BCON_BINARY(main_id, UUIDLEN),
        "status", BCON_INT32((int)LIVE_STATUS_ALIVE),
        "create_milli_timestamp", "{",
            "$lt", BCON_INT64(*create_milli_timestamp),
        "}"
        );

    if(!error_code) {
        error_code = db_count(MONGO_COMMENT, count_query_lt, &count_lt_create_milli_timestamp);
    }

    *n_comment = count_eq_create_milli_timestamp + count_lt_create_milli_timestamp;

    bson_safe_destroy(&key);
    bson_safe_destroy(&fields);
    bson_safe_destroy(&sort);
    bson_safe_destroy(&result);
    bson_safe_destroy(&count_query_lt);
    bson_safe_destroy(&count_query_eq);

    return error_code;
}

Err
read_comments_until_newest_to_bsons(UUID main_id, time64_t create_milli_timestamp, char *poster, bson_t *fields, int max_n_comment, bson_t **b_comments, int *n_comment)
{
    Err error_code = S_OK;

    bson_t *query_lt = BCON_NEW(
        "main_id", BCON_BINARY(main_id, UUIDLEN),
        "status", BCON_INT32((int)LIVE_STATUS_ALIVE),
        "create_milli_timestamp", "{",
            "$lt", BCON_INT64(create_milli_timestamp),
        "}"
        );

    bson_t *sort = BCON_NEW(
        "create_milli_timestamp", BCON_INT32(1),
        "poster", BCON_INT32(1)
    );
    
    bson_t **p_b_comments = b_comments;
    int n_comment_lt_create_milli_timestamp = 0;
    error_code = db_find(MONGO_COMMENT, query_lt, fields, sort, max_n_comment, &n_comment_lt_create_milli_timestamp, p_b_comments);
    p_b_comments += n_comment_lt_create_milli_timestamp;
    max_n_comment -= n_comment_lt_create_milli_timestamp;

    bson_t *query_eq = BCON_NEW(
        "main_id", BCON_BINARY(main_id, UUIDLEN),
        "status", BCON_INT32((int)LIVE_STATUS_ALIVE),
        "create_milli_timestamp", BCON_INT64(create_milli_timestamp),
        "poster", "{",
            "$lte", BCON_BINARY((unsigned char *)poster, IDLEN),
        "}"
        );
    int n_comment_eq_create_milli_timestamp = 0;
    if(!error_code) {
        error_code = db_find(MONGO_COMMENT, query_eq, fields, sort, max_n_comment, &n_comment_eq_create_milli_timestamp, p_b_comments);
    }

    *n_comment = n_comment_lt_create_milli_timestamp + n_comment_eq_create_milli_timestamp;

    // free
    bson_safe_destroy(&query_lt);
    bson_safe_destroy(&sort);
    bson_safe_destroy(&query_eq);

    return error_code;
}

Err
sort_b_comments_by_comment_id(bson_t **b_comments, int n_comment)
{
    qsort(b_comments, n_comment, sizeof(bson_t *), _cmp_b_comments_by_comment_id);

    return S_OK;        
}

int
_cmp_b_comments_by_comment_id(const void *a, const void *b)
{
    Err error_code = S_OK;
    bson_t **p_b_comment_a = (bson_t **)a;
    bson_t **p_b_comment_b = (bson_t **)b;
    bson_t *b_comment_a = *p_b_comment_a;
    bson_t *b_comment_b = *p_b_comment_b;

    UUID comment_id_a = {};
    UUID comment_id_b = {};
    int len = 0;

    error_code = bson_get_value_bin(b_comment_a, "the_id", UUIDLEN, (char *)comment_id_a, &len);
    if(error_code) comment_id_a[0] = 0;
    error_code = bson_get_value_bin(b_comment_b, "the_id", UUIDLEN, (char *)comment_id_b, &len);
    if(error_code) comment_id_b[0] = 0;

    return strncmp((char *)comment_id_a, (char *)comment_id_b, UUIDLEN);
}


Err
_get_comment_info_by_main_deal_with_result(bson_t *result, int n_result, int *n_total_comment, int *total_len)
{
    if (!n_result) {
        *n_total_comment = 0;
        *total_len = 0;
        return S_OK;
    }

    Err error_code = S_OK;

    error_code = bson_get_value_int32(result, "count", n_total_comment);

    if (!error_code) {
        error_code = bson_get_value_int32(result, "len", total_len);
    }

    return error_code;
}

Err
_read_comments_get_b_comments(bson_t **b_comments, UUID main_id, time64_t create_milli_timestamp, char *poster, enum ReadCommentsOpType op_type, int max_n_comment, enum MongoDBId mongo_db_id, int *n_comment)
{
    Err error_code = S_OK;

    int n_comment_same_create_milli_timestamp = 0;

    bson_t **p_b_comments = b_comments;
    error_code = _read_comments_get_b_comments_same_create_milli_timestamp(p_b_comments, main_id, create_milli_timestamp, poster, op_type, max_n_comment, mongo_db_id, &n_comment_same_create_milli_timestamp);
    if(error_code == S_ERR_NOT_EXISTS) error_code = S_OK;

    if(!error_code) {
        p_b_comments += n_comment_same_create_milli_timestamp;
        max_n_comment -= n_comment_same_create_milli_timestamp;
    }

    int n_comment_diff_create_milli_timestamp = 0;
    if(!error_code && max_n_comment > 0) {
        error_code = _read_comments_get_b_comments_diff_create_milli_timestamp(p_b_comments, main_id, create_milli_timestamp, op_type, max_n_comment, mongo_db_id, &n_comment_diff_create_milli_timestamp);
        if(error_code == S_ERR_NOT_EXISTS) error_code = S_OK;
    }

    *n_comment = n_comment_same_create_milli_timestamp + n_comment_diff_create_milli_timestamp;

    if(!error_code && (op_type == READ_COMMENTS_OP_TYPE_LT || op_type == READ_COMMENTS_OP_TYPE_LTE)) {
        error_code = _reverse_b_comments(b_comments, *n_comment);
    }

    return error_code;
}


Err
_read_comments_get_b_comments_same_create_milli_timestamp(bson_t **b_comments, UUID main_id, time64_t create_milli_timestamp, char *poster, enum ReadCommentsOpType op_type, int max_n_comment, enum MongoDBId mongo_db_id, int *n_comment)
{
    Err error_code = S_OK;
    int order = (op_type == READ_COMMENTS_OP_TYPE_GT || op_type == READ_COMMENTS_OP_TYPE_GTE) ? 1 : -1;

    // get same create_milli_timestamp but not same poster
    bson_t *key = BCON_NEW(
        "main_id", BCON_BINARY(main_id, UUIDLEN),
        "status", BCON_INT32(LIVE_STATUS_ALIVE),
        "create_milli_timestamp", BCON_INT64(create_milli_timestamp),
        "poster", "{",
            _read_comments_op_type[op_type], BCON_BINARY((unsigned char *)poster, IDLEN),
        "}"
    );

    if (key == NULL) error_code = S_ERR_INIT;

    bson_t *sort = BCON_NEW(
        "poster", BCON_INT32(order)
    );
    
    if(sort == NULL) error_code = S_ERR_INIT;

    if(!error_code) {
        error_code = _read_comments_get_b_comments_core(b_comments, key, sort, op_type, max_n_comment, mongo_db_id, n_comment);
    }

    bson_safe_destroy(&key);
    bson_safe_destroy(&sort);

    return error_code;
}

Err
_read_comments_get_b_comments_diff_create_milli_timestamp(bson_t **b_comments, UUID main_id, time64_t create_milli_timestamp, enum ReadCommentsOpType op_type, int max_n_comment, enum MongoDBId mongo_db_id, int *n_comment)
{
    Err error_code = S_OK;
    int order = (op_type == READ_COMMENTS_OP_TYPE_GT || op_type == READ_COMMENTS_OP_TYPE_GTE) ? 1 : -1;
    if(op_type == READ_COMMENTS_OP_TYPE_GTE) {
        op_type = READ_COMMENTS_OP_TYPE_GT;
    }

    if(op_type == READ_COMMENTS_OP_TYPE_LTE) {
        op_type = READ_COMMENTS_OP_TYPE_LT;
    }

    // get same create_milli_timestamp but not same poster
    bson_t *key = BCON_NEW(
        "main_id", BCON_BINARY(main_id, UUIDLEN),
        "status", BCON_INT32(LIVE_STATUS_ALIVE),
        "create_milli_timestamp", "{",
            _read_comments_op_type[op_type], BCON_INT64(create_milli_timestamp),
        "}"
    );

    if (key == NULL) error_code = S_ERR;

    bson_t *sort = BCON_NEW(
        "create_milli_timestamp", BCON_INT32(order),
        "poster", BCON_INT32(order)
    );
    
    if(sort == NULL) error_code = S_ERR;

    if(!error_code) {
        error_code = _read_comments_get_b_comments_core(b_comments, key, sort, op_type, max_n_comment, mongo_db_id, n_comment);
    }

    bson_safe_destroy(&key);
    bson_safe_destroy(&sort);

    return error_code;
}

Err
_read_comments_get_b_comments_core(bson_t **b_comments, bson_t *key, bson_t *sort, enum ReadCommentsOpType op_type, int max_n_comment, enum MongoDBId mongo_db_id, int *n_comment)
{
    Err error_code = db_find(mongo_db_id, key, NULL, sort, max_n_comment, n_comment, b_comments);

    int tmp_n_comment = *n_comment;

    Err error_code_ensure_order = S_OK;
    if (!error_code) {
        error_code_ensure_order = _ensure_b_comments_order(b_comments, tmp_n_comment, op_type);
    }

    if (!error_code && error_code_ensure_order) {
        error_code = _sort_b_comments_order(b_comments, tmp_n_comment, op_type);
    }

    return error_code;
}

Err
_ensure_b_comments_order(bson_t **b_comments, int n_comment, enum ReadCommentsOpType op_type)
{
    Err error_code = S_OK;
    bson_t **p_b_comments = b_comments;
    bson_t **p_next_b_comments = b_comments + 1;
    int cmp = 0;
    int (*_cmp)(const void *a, const void *b) = (op_type == READ_COMMENTS_OP_TYPE_LT || op_type == READ_COMMENTS_OP_TYPE_LTE) ? _cmp_b_comments_descending : _cmp_b_comments_ascending;

    for (int i = 0; i < n_comment - 1; i++, p_b_comments++, p_next_b_comments++) {
        cmp = _cmp(p_b_comments, p_next_b_comments);
        if (cmp > 0) {
            error_code = S_ERR;
            break;
        }
    }

    return error_code;
}

Err
_sort_b_comments_order(bson_t **b_comments, int n_comment, enum ReadCommentsOpType op_type)
{
    int (*_cmp)(const void *a, const void *b) = (op_type == READ_COMMENTS_OP_TYPE_LT || op_type == READ_COMMENTS_OP_TYPE_LTE) ? _cmp_b_comments_descending : _cmp_b_comments_ascending;

    qsort(b_comments, n_comment, sizeof(bson_t *), _cmp);

    return S_OK;
}

int
_cmp_b_comments_ascending(const void *a, const void *b)
{
    bson_t **p_b_comment_a = (bson_t **)a;
    bson_t *b_comment_a = *p_b_comment_a;
    bson_t **p_b_comment_b = (bson_t **)b;
    bson_t *b_comment_b = *p_b_comment_b;

    time64_t create_milli_timestamp_a = 0;
    time64_t create_milli_timestamp_b = 0;

    char poster_a[IDLEN + 1] = {};
    char poster_b[IDLEN + 1] = {};

    Err error_code;
    error_code = bson_get_value_int64(b_comment_a, "create_milli_timestamp", (long *)&create_milli_timestamp_a);
    if (error_code) create_milli_timestamp_a = -1;

    error_code = bson_get_value_int64(b_comment_b, "create_milli_timestamp", (long *)&create_milli_timestamp_b);
    if (error_code) create_milli_timestamp_b = -1;

    if (create_milli_timestamp_a != create_milli_timestamp_b) {
        return create_milli_timestamp_a - create_milli_timestamp_b;
    }

    int len;
    error_code = bson_get_value_bin(b_comment_a, "poster", IDLEN, poster_a, &len);
    if (error_code) poster_a[0] = 0;

    error_code = bson_get_value_bin(b_comment_b, "poster", IDLEN, poster_b, &len);
    if (error_code) poster_b[0] = 0;

    return strncmp(poster_a, poster_b, IDLEN);
}

int
_cmp_b_comments_descending(const void *a, const void *b)
{
    bson_t **p_b_comment_a = (bson_t **)a;
    bson_t *b_comment_a = *p_b_comment_a;
    bson_t **p_b_comment_b = (bson_t **)b;
    bson_t *b_comment_b = *p_b_comment_b;

    time64_t create_milli_timestamp_a = 0;
    time64_t create_milli_timestamp_b = 0;

    char poster_a[IDLEN + 1] = {};
    char poster_b[IDLEN + 1] = {};

    Err error_code;
    error_code = bson_get_value_int64(b_comment_a, "create_milli_timestamp", (long *)&create_milli_timestamp_a);
    if (error_code) create_milli_timestamp_a = -1;

    error_code = bson_get_value_int64(b_comment_b, "create_milli_timestamp", (long *)&create_milli_timestamp_b);
    if (error_code) create_milli_timestamp_b = -1;

    if (create_milli_timestamp_a != create_milli_timestamp_b) {
        return create_milli_timestamp_b - create_milli_timestamp_a;
    }

    int len;
    error_code = bson_get_value_bin(b_comment_a, "poster", IDLEN, poster_a, &len);
    if (error_code) bzero(poster_a, IDLEN + 1);

    error_code = bson_get_value_bin(b_comment_b, "poster", IDLEN, poster_b, &len);
    if (error_code) bzero(poster_b, IDLEN + 1);

    return strncmp(poster_b, poster_a, IDLEN);
}

Err
_reverse_b_comments(bson_t **b_comments, int n_comment)
{
    int half_n_comment = (n_comment + 1) / 2; // 8 as 4, 7 as 4
    bson_t **p_b_comments = b_comments;
    bson_t **p_end_b_comments = b_comments + n_comment - 1;
    bson_t *tmp;
    for(int i = 0; i < half_n_comment; i++, p_b_comments++, p_end_b_comments--) {
        tmp = *p_b_comments;
        *p_b_comments = *p_end_b_comments;
        *p_end_b_comments = tmp;        
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
_serialize_comment_bson(Comment *comment, bson_t **comment_bson)
{
    *comment_bson = BCON_NEW(
                        "version", BCON_INT32(comment->version),
                        "the_id", BCON_BINARY(comment->the_id, UUIDLEN),
                        "main_id", BCON_BINARY(comment->main_id, UUIDLEN),
                        "comment_reply_id", BCON_BINARY(comment->comment_reply_id, UUIDLEN),
                        "n_comment_reply_line", BCON_INT32(comment->n_comment_reply_line),
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
    error_code = bson_get_value_int32(comment_bson, "version", (int *)&comment->version);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "the_id", UUIDLEN, (char *)comment->the_id, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "main_id", UUIDLEN, (char *)comment->main_id, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "comment_reply_id", UUIDLEN, (char *)comment->comment_reply_id, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(comment_bson, "n_comment_reply_line", &comment->n_comment_reply_line);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(comment_bson, "status", (int *)&comment->status);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(comment_bson, "comment_type", (int *)&comment->comment_type);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(comment_bson, "karma", (int *)&comment->karma);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "status_updater", IDLEN, comment->status_updater, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "status_update_ip", IPV4LEN, comment->status_update_ip, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "poster", IDLEN, comment->poster, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "ip", IPV4LEN, comment->ip, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_int64(comment_bson, "create_milli_timestamp", (long *)&comment->create_milli_timestamp);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "updater", IDLEN, comment->updater, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "update_ip", IPV4LEN, comment->update_ip, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_int64(comment_bson, "update_milli_timestamp", (long *)&comment->update_milli_timestamp);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(comment_bson, "len", &comment->len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(comment_bson, "buf", comment->max_buf_len, comment->buf, &len);
    if (error_code) return error_code;

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
_deserialize_comment_bson_with_buf(bson_t *comment_bson, Comment *comment)
{
    Err error_code = S_OK;
    if (comment->buf) return S_ERR;

    int len = 0;
    error_code = bson_get_value_int32(comment_bson, "len", &len);
    if (error_code) return error_code;

    comment->buf = malloc(len);
    comment->max_buf_len = len;
    comment->len = 0;

    return _deserialize_comment_bson(comment_bson, comment);
}
