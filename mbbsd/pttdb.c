#include "pttdb.h"
#include "pttdb_internal.h"

/**
 * @brief Get the lines of the post to be able to show the percentage.
 * @details Get the lines of the post to be able to show the percentage.
 *          XXX maybe we need to exclude n_line_comment_reply to simplify the indication of the line no.
 *          XXX Simplification: We count only the lines of main the numbers of comments.
 *                              It is with high probability that the users accept the modification.
 * @param main_id main_id for the post
 * @param n_line n_line (to-compute)
 * @return Err
 */

Err
n_line_post(UUID main_id, int *n_line) {
    Err error_code = S_OK;
    int the_line_main = 0;
    int the_line_comments = 0;
    int the_line_comment_reply = 0;
    int n_comment_reply = 0;
    int len_comment_reply = 0;

    error_code = n_line_main(main_id, &the_line_main);
    if (error_code) return error_code;

    error_code = get_comment_count_by_main(main_id, &the_line_comments);
    if (error_code) return error_code;

    error_code = get_comment_reply_info_by_main(main_id, &n_comment_reply, &the_line_comment_reply, &len_comment_reply);
    if (error_code) return error_code;

    *n_line = the_line_main + the_line_comments + the_line_comment_reply;

    return S_OK;
}


Err
get_file_info_by_main(UUID main_id, FileInfo *file_info)
{
    Err error_code = S_OK;

    error_code = _get_file_info_by_main_get_main_info(main_id, file_info);
    if(!error_code) {
        error_code = _get_file_info_by_main_get_content_block_info(file_info);
    }
    if(!error_code) {
        error_code = _get_file_info_by_main_get_comment_comment_reply_info(main_id, file_info);
    }

    return error_code;
}

Err
destroy_file_info(FileInfo *file_info)
{
    if(!file_info) return S_OK;

    if(file_info->content_block_info) {
        free(file_info->content_block_info);
    }
    if(file_info->comment_comment_reply_info) {
        free(file_info->comment_comment_reply_info);
    }
    file_info->comment_comment_reply_info = NULL;
    file_info->content_block_info = NULL;
    file_info->n_main_line = 0;
    file_info->n_main_block = 0;
    file_info->n_comment = 0;

    return S_OK;
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param main_id [description]
 * @param file_info [!] need to do destory
 */
Err
_get_file_info_by_main_get_main_info(UUID main_id, FileInfo *file_info)
{
    Err error_code = S_OK;
    bson_t *b_main = NULL;
    bson_t *fields = BCON_NEW(
        "_id", BCON_BOOL(false),
        "updater", BCON_BOOL(true),
        "update_milli_timestamp", BCON_BOOL(true),
        "content_id", BCON_BOOL(true),
        "n_total_line", BCON_BOOL(true),
        "n_total_block", BCON_BOOL(true)
        );

    memcpy(file_info->main_id, main_id, UUIDLEN);
    int len = 0;
    error_code = read_main_header_to_bson(main_id, fields, &b_main);
    if(!error_code) {
        error_code = bson_get_value_bin(b_main, "updater", IDLEN, file_info->main_updater, &len);
    }
    if(!error_code) {
        error_code = bson_get_value_int64(b_main, "update_milli_timestamp", (long *)&file_info->main_update_milli_timestamp);
    }
    if(!error_code) {
        error_code = bson_get_value_bin(b_main, "content_id", UUIDLEN, (char *)file_info->main_content_id, &len);
    }
    if(!error_code) {
        error_code = bson_get_value_int32(b_main, "n_total_line", &file_info->n_main_line);    
    }
    if(!error_code) {
        error_code = bson_get_value_int32(b_main, "n_total_block", &file_info->n_main_block);
    }

    bson_safe_destroy(&b_main);
    bson_safe_destroy(&fields);

    return error_code;
}


Err
_get_file_info_by_main_get_content_block_info(FileInfo *file_info)
{
    Err error_code = S_OK;

    // get bson data
    bson_t *content_block_fields = BCON_NEW(
            "_id", BCON_BOOL(false),
            "block_id", BCON_BOOL(true),
            "n_line", BCON_BOOL(true)
        );

    int n_content_block = 0;
    bson_t **b_content_blocks = malloc(sizeof(bson_t *) * file_info->n_main_block);
    if(!b_content_blocks) error_code = S_ERR;

    if(!error_code) {
        error_code = read_content_blocks_to_bsons(file_info->main_content_id, content_block_fields, file_info->n_main_block, MONGO_MAIN_CONTENT, b_content_blocks, &n_content_block);
        if(n_content_block != file_info->n_main_block) error_code = S_ERR; // not matched.
    }

    // XXX init file-info content-block!
    if(!error_code) {
        file_info->content_block_info = malloc(sizeof(ContentBlockInfo) * file_info->n_main_block);
        if(!file_info->content_block_info) error_code = S_ERR_INIT;
    }
    
    // bson data to content block info
    bson_t **p_b_content_blocks = b_content_blocks;
    ContentBlockInfo *p_content_block_info = file_info->content_block_info;
    if(!error_code) {
        for(int i = 0; i < n_content_block; i++, p_b_content_blocks++, p_content_block_info++) {
            error_code = bson_get_value_int32(*p_b_content_blocks, "block_id", &p_content_block_info->block_id);
            if(error_code) break;
            error_code = bson_get_value_int32(*p_b_content_blocks, "n_line", &p_content_block_info->n_line);
            if(error_code) break;
        }
    }

    // reset n_main_block and n_main_line if error
    if(error_code) {
        file_info->n_main_block = 0;
        file_info->n_main_line = 0;
    }

    // free
    bson_safe_destroy(&content_block_fields);

    safe_free_b_list(&b_content_blocks, n_content_block);

    return error_code;
}

Err
_get_file_info_by_main_get_comment_comment_reply_info(UUID main_id, FileInfo *file_info)
{
    Err error_code = S_OK;

    UUID comment_id = {};
    time64_t create_milli_timestamp = 0;
    char poster[IDLEN + 1] = {};

    bson_t **b_comments = NULL;    
    int n_expected_comment = 0;
    int n_comment = 0;

    // get newest-comment and n_expected_comments
    bson_t *comment_fields = BCON_NEW(
        "_id", BCON_BOOL(false),
        "the_id", BCON_BOOL(true),
        "create_milli_timestamp", BCON_BOOL(true),
        "poster", BCON_BOOL(true)
        );
    error_code = get_newest_comment(main_id, comment_id, &create_milli_timestamp, poster, &n_expected_comment);

    // get comments
    if(!error_code) {
        b_comments = malloc(sizeof(bson_t *) * n_expected_comment);
        error_code = read_comments_until_newest_to_bsons(main_id, create_milli_timestamp, poster, comment_fields, n_expected_comment, b_comments, &n_comment);
    }

    if(!error_code) {
        error_code = sort_b_comments_by_comment_id(b_comments, n_comment);
    }

    // XXX init file_info->comment_comment_reply_info
    if(!error_code) {
        file_info->comment_comment_reply_info = malloc(sizeof(CommentCommentReplyInfo) * n_comment);
        if(!file_info->comment_comment_reply_info) error_code = S_ERR;
    }
    if(!error_code) {
        file_info->n_comment = n_comment;
    }

    int len = 0;
    bson_t **p_b_comments = b_comments;
    CommentCommentReplyInfo *p_comment_comment_reply_info = file_info->comment_comment_reply_info;
    if(!error_code) {
        for(int i = 0; i < n_comment; i++, p_b_comments++, p_comment_comment_reply_info++) {
            error_code = bson_get_value_bin(*p_b_comments, "the_id", UUIDLEN, (char *)p_comment_comment_reply_info->comment_id, &len);
            if(error_code) break;
            error_code = bson_get_value_int64(*p_b_comments, "create_milli_timestamp", (long *)&p_comment_comment_reply_info->comment_create_milli_timestamp);
            if(error_code) break;
            error_code = bson_get_value_bin(*p_b_comments, "poster", IDLEN, p_comment_comment_reply_info->comment_poster, &len);
            if(error_code) break;
            p_comment_comment_reply_info->n_comment_reply_line = 0;
        }
    }

    // get comment-replys
    int next_i;
    const int block = 1000;
    p_comment_comment_reply_info = file_info->comment_comment_reply_info;
    if(!error_code) {
        for(int i = 0; i < n_comment; i += block, p_comment_comment_reply_info += block) {
            next_i = (n_comment < i + block) ? n_comment : (i + block);
            error_code = _get_file_info_by_main_get_comment_reply_info(p_comment_comment_reply_info, i, next_i);
            if(error_code == S_ERR_NOT_EXISTS) error_code = S_OK;
            if(error_code) break;
        }
    }

    //sort comment_comment_reply_info
    if(!error_code) {
        error_code = _sort_comment_comment_reply_info_by_comment_create_milli_timestamp(file_info->comment_comment_reply_info, file_info->n_comment);
    }

    // free
    bson_safe_destroy(&comment_fields);

    safe_free_b_list(&b_comments, n_comment);

    if(error_code == S_ERR_NOT_EXISTS) error_code = S_OK;

    return error_code;
}

Err
_get_file_info_by_main_get_comment_reply_info(CommentCommentReplyInfo *comment_comment_reply_info, int start_i, int next_i)
{
    Err error_code = S_OK;
    int n_comment_comment_reply_info = next_i - start_i;

    CommentCommentReplyInfo *p_comment_comment_reply_info = comment_comment_reply_info;

    // construct query
    bson_t *q_array = bson_new();
    bson_t child;
    char buf[16];
    const char *key;
    size_t keylen;
    bool status;
    BSON_APPEND_ARRAY_BEGIN(q_array, "$in", &child);
    for(int i = start_i; i < next_i; i++, p_comment_comment_reply_info++) {
        keylen = bson_uint32_to_string(i, &key, buf, sizeof(buf));
        status = bson_append_bin(&child, key, (int)keylen, p_comment_comment_reply_info->comment_id, UUIDLEN);
        if (!status) {
            error_code = S_ERR_INIT;
            break;
        }
    }
    bson_append_array_end(q_array, &child);

    bson_t *query = BCON_NEW(
        "comment_id", BCON_DOCUMENT(q_array)
        );

    // get comment-replys
    bson_t **b_comment_replys = malloc(sizeof(bson_t *) * n_comment_comment_reply_info);
    if(!b_comment_replys) error_code = S_ERR;

    int n_comment_reply = 0;
    bson_t *fields = BCON_NEW(
        "_id", BCON_BOOL(false),
        "comment_id", BCON_BOOL(true),
        "the_id", BCON_BOOL(true),
        "n_line", BCON_BOOL(true)
        );
    if(!error_code) {
        error_code = read_comment_replys_by_query_to_bsons(query, fields, n_comment_comment_reply_info, b_comment_replys, &n_comment_reply);
    }

    if(!error_code) {
        error_code = sort_b_comment_replys_by_comment_id(b_comment_replys, n_comment_reply);
    }

    // align comment_replys to comment_comment_reply_info
    if(!error_code) {
        error_code = _get_file_info_by_main_align_comment_comment_reply_info(comment_comment_reply_info, b_comment_replys, n_comment_comment_reply_info, n_comment_reply);

    }

    //free
    safe_free_b_list(&b_comment_replys, n_comment_reply);

    bson_safe_destroy(&fields);
    bson_safe_destroy(&query);
    bson_safe_destroy(&q_array);

    return error_code;
}

Err
_get_file_info_by_main_align_comment_comment_reply_info(CommentCommentReplyInfo *comment_comment_reply_info, bson_t **b_comment_replys, int n_comment_comment_reply_info, int n_comment_reply)
{
    Err error_code = S_OK;

    UUID tmp_comment_id = {};
    int len = 0;
    for(int i = 0; i < n_comment_reply; i++) {
        bson_get_value_bin(b_comment_replys[i], "comment_id", UUIDLEN, (char *)tmp_comment_id, &len);
    }

    // init
    int i = 0;
    int j = 0;
    CommentCommentReplyInfo *p_comment_comment_reply_info = comment_comment_reply_info;
    bson_t **p_b_comment_replys = b_comment_replys;
    int tmp_cmp = 0;

    UUID *comment_id = &p_comment_comment_reply_info->comment_id;
    UUID comment_reply_comment_id = {};
    error_code = bson_get_value_bin(*p_b_comment_replys, "comment_id", UUIDLEN, (char *)comment_reply_comment_id, &len);
    if(error_code) return error_code;

    // while-loop
    while(i < n_comment_comment_reply_info && j < n_comment_reply) {
        tmp_cmp = strncmp((char *)*comment_id, (char *)comment_reply_comment_id, UUIDLEN);

        if(tmp_cmp < 0) {
            // increase comment_comment_reply_info
            i++;
            p_comment_comment_reply_info++;
            if(i < n_comment_comment_reply_info) {
                comment_id = &p_comment_comment_reply_info->comment_id;
            }
        }
        else if(tmp_cmp > 0) {
            // increase comment_replys
            j++;
            p_b_comment_replys++;
            if(j < n_comment_reply) {
                error_code = bson_get_value_bin(*p_b_comment_replys, "comment_id", UUIDLEN, (char *)comment_reply_comment_id, &len);
                if(error_code) break;
            }
        }
        else {
            // setup
            error_code = bson_get_value_bin(*p_b_comment_replys, "the_id", UUIDLEN, (char *)p_comment_comment_reply_info->comment_reply_id, &len);
            if(error_code) break;
            error_code = bson_get_value_int32(*p_b_comment_replys, "n_line", &p_comment_comment_reply_info->n_comment_reply_line);
            if(error_code) break;

            // increase comment_comment_reply_info
            i++;
            p_comment_comment_reply_info++;

            if(i < n_comment_comment_reply_info) {
                comment_id = &p_comment_comment_reply_info->comment_id;
            }

            // increase comment_replys
            j++;
            p_b_comment_replys++;
            if(j < n_comment_reply) {
                error_code = bson_get_value_bin(*p_b_comment_replys, "comment_id", UUIDLEN, (char *)comment_reply_comment_id, &len);
                if(error_code) break;
            }
        }
    }

    return error_code;
}

Err
_sort_comment_comment_reply_info_by_comment_create_milli_timestamp(CommentCommentReplyInfo *comment_comment_reply_info, int n_comment_comment_reply_info)
{
    qsort(comment_comment_reply_info, n_comment_comment_reply_info, sizeof(CommentCommentReplyInfo),_cmp_comment_comment_reply_info_by_comment_create_milli_timestamp);

    return S_OK;
}

int
_cmp_comment_comment_reply_info_by_comment_create_milli_timestamp(const void *a, const void *b)
{
    CommentCommentReplyInfo *tmp_a = (CommentCommentReplyInfo *)a;
    CommentCommentReplyInfo *tmp_b = (CommentCommentReplyInfo *)b;

    if(tmp_a->comment_create_milli_timestamp != tmp_b->comment_create_milli_timestamp) {
        return tmp_a->comment_create_milli_timestamp - tmp_b->comment_create_milli_timestamp;
    }

    return strcmp(tmp_a->comment_poster, tmp_b->comment_poster);
}