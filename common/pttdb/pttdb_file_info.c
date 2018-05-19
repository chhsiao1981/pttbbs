#include "cmpttdb/pttdb_file_info.h"
#include "cmpttdb/pttdb_file_info_private.h"

bool
file_info_is_first_block_ne(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset)
{
    bool is_first_block = false;
    file_info_is_first_block(file_info, content_type, comment_offset, block_offset, &is_first_block);

    return is_first_block;
}


Err
file_info_is_first_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, bool *is_first_block)
{
    ContentBlockInfo *p_content = NULL;
    Err error_code = file_info_get_content_block(file_info, content_type, comment_offset, block_offset, &p_content);
    if(error_code) return error_code;

    return _file_info_is_first_block_core(file_info, p_content, block_offset, is_first_block);
}

Err
_file_info_is_first_block_core(FileInfo *file_info GCC_UNUSED, ContentBlockInfo *content, int block_offset, bool *is_first_block)
{
    if(!block_offset) {
        *is_first_block = true;
        return S_OK;
    }

    ContentBlockInfo *p_content = content - 1;
    for(int i = block_offset - 1; i >= 0; i--, p_content--) {
        if(p_content->n_line) {
            *is_first_block = false;
            return S_OK;
        }
    }

    *is_first_block = true;
    return S_OK;
}

bool
file_info_is_last_block_ne(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset)
{
    bool is_last_block = false;
    file_info_is_last_block(file_info, content_type, comment_offset, block_offset, &is_last_block);

    return is_last_block;
}

Err
file_info_is_last_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, bool *is_last_block)
{
    ContentBlockInfo *p_content = NULL;
    Err error_code = file_info_get_content_block(file_info, content_type, comment_offset, block_offset, &p_content);
    if(error_code) return error_code;

    int n_block = 0;
    error_code = file_info_get_n_block(file_info, content_type, comment_offset, &n_block);

    return _file_info_is_last_block_core(file_info, p_content, block_offset, n_block, is_last_block);
}

Err
_file_info_is_last_block_core(FileInfo *file_info GCC_UNUSED, ContentBlockInfo *content, int block_offset, int n_block, bool *is_last_block)
{
    if(block_offset == n_block - 1) {
        *is_last_block = true;
        return S_OK;
    }

    ContentBlockInfo *p_content = content + 1;
    for(int i = block_offset + 1; i < n_block; i++, p_content++) {
        if(p_content->n_line) {
            *is_last_block = false;
            return S_OK;
        }
    }

    *is_last_block = true;
    return S_OK;
}

Err
file_info_pre_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, int *new_block_offset)
{
    if(block_offset == 0) return S_ERR_NOT_EXISTS;

    ContentBlockInfo *p_content = NULL;
    Err error_code = file_info_get_content_block(file_info, content_type, comment_offset, block_offset, &p_content);
    if(error_code) return error_code;

    p_content--;
    for(int i = block_offset - 1; i >= 0; i--, p_content--) {
        if(p_content->n_line) {
            *new_block_offset = i;
            return S_OK;
        }
    }

    return S_ERR_NOT_EXISTS;
}

Err
file_info_next_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, int *new_block_offset)
{
    int n_block = 0;
    Err error_code = file_info_get_n_block(file_info, content_type, comment_offset, &n_block);
    if(error_code) return error_code;

    if(block_offset == n_block - 1) return S_ERR_NOT_EXISTS;

    ContentBlockInfo *p_content = NULL;
    error_code = file_info_get_content_block(file_info, content_type, comment_offset, 0, &p_content);
    if(error_code) return error_code;

    p_content++;
    for(int i = block_offset + 1; i < n_block; i++, p_content++) {
        if(p_content->n_line) {
            *new_block_offset = i;
            return S_OK;
        }
    }

    return S_ERR_NOT_EXISTS;
}

Err
file_info_first_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int *new_block_offset)
{
    int n_block = 0;
    Err error_code = file_info_get_n_block(file_info, content_type, comment_offset, &n_block);
    if(error_code) return error_code;

    ContentBlockInfo *p_content = NULL;
    error_code = file_info_get_content_block(file_info, content_type, comment_offset, 0, &p_content);
    if(error_code) return error_code;

    for(int i = 0; i < n_block; i++, p_content++) {
        if(p_content->n_line) {
            *new_block_offset = i;
            return S_OK;
        }
    }

    return S_ERR_NOT_EXISTS;
}

Err
file_info_last_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int *new_block_offset)
{
    int n_block = 0;
    Err error_code = file_info_get_n_block(file_info, content_type, comment_offset, &n_block);
    if(error_code) return error_code;

    ContentBlockInfo *p_content = NULL;
    error_code = file_info_get_content_block(file_info, content_type, comment_offset, n_block - 1, &p_content);
    if(error_code) return error_code;

    for(int i = n_block - 1; i >= 0; i--, p_content--) {
        if(p_content->n_line) {
            *new_block_offset = i;
            return S_OK;
        }
    }

    return S_ERR_NOT_EXISTS;
}

bool
file_info_is_first_line_ne(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, int line_offset)
{
    bool is_first_line = false;

    file_info_is_first_line(file_info, content_type, comment_offset, block_offset, line_offset, &is_first_line);

    return is_first_line;
}

Err
file_info_is_first_line(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, int line_offset, bool *is_first_line)
{
    switch (content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        return _file_info_is_first_line_main(file_info, content_type, comment_offset, block_offset, line_offset, is_first_line);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT:
        return _file_info_is_first_line_comment(file_info, content_type, comment_offset, block_offset, line_offset, is_first_line);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        *is_first_line = false;
        return S_OK;
        break;
    default:
        break;
    }

    return S_OK;
}

Err
_file_info_is_first_line_main(FileInfo *file_info, enum PttDBContentType content_type GCC_UNUSED, int comment_offset GCC_UNUSED, int block_offset, int line_offset, bool *is_first_line)
{
    if(line_offset != 0) {
        *is_first_line = false;
        return S_OK;
    }

    // block == 0 and line == 0
    if(block_offset == 0) {
        *is_first_line = true;
        return S_OK;
    }

    ContentBlockInfo *p_content = file_info->main_blocks + block_offset - 1;
    for (int i = block_offset - 1; i >= 0; i--, p_content--) {
        if (p_content->n_line) {
            *is_first_line = false;
            return S_OK;
        }
    }

    *is_first_line = true;
    return S_OK;
}

Err
_file_info_is_first_line_comment(FileInfo *file_info, enum PttDBContentType content_type GCC_UNUSED, int comment_offset, int block_offset GCC_UNUSED, int line_offset GCC_UNUSED, bool *is_first_line)
{
    *is_first_line = comment_offset == 0 && file_info->n_main_line == 0 ? true : false;

    return S_OK;
}

Err
file_info_is_last_line(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, int line_offset, bool *is_last_line)
{
    switch (content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        return _file_info_is_last_line_main(file_info, content_type, comment_offset, block_offset, line_offset, is_last_line);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT:
        return _file_info_is_last_line_comment(file_info, content_type, comment_offset, block_offset, line_offset, is_last_line);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        return _file_info_is_last_line_comment_reply(file_info, content_type, comment_offset, block_offset, line_offset, is_last_line);
        break;
    default:
        break;
    }

    return S_OK;
}

Err
_file_info_is_last_line_main(FileInfo *file_info, enum PttDBContentType content_type GCC_UNUSED, int comment_offset GCC_UNUSED, int block_offset, int line_offset, bool *is_last_line)
{
    if(file_info->n_comment) {
        *is_last_line = false;
        return S_OK;
    }

    ContentBlockInfo *p_content = file_info->main_blocks + block_offset;

    return _file_info_is_last_line_content(p_content, block_offset, line_offset, file_info->n_main_block, is_last_line);
}

Err
_file_info_is_last_line_comment(FileInfo *file_info, enum PttDBContentType content_type GCC_UNUSED, int comment_offset, int block_offset GCC_UNUSED, int line_offset GCC_UNUSED, bool *is_last_line)
{
    *is_last_line = comment_offset == file_info->n_comment - 1 && file_info->comments[comment_offset].n_comment_reply_total_line == 0 ? true : false;

    return S_OK;
}

Err
_file_info_is_last_line_comment_reply(FileInfo *file_info, enum PttDBContentType content_type GCC_UNUSED, int comment_offset, int block_offset, int line_offset, bool *is_last_line)
{
    if(comment_offset != file_info->n_comment - 1) {
        *is_last_line = false;
        return S_OK;
    }

    CommentInfo *p_comment = file_info->comments + comment_offset;
    ContentBlockInfo *p_content = p_comment->comment_reply_blocks + block_offset;

    return _file_info_is_last_line_content(p_content, block_offset, line_offset, p_comment->n_comment_reply_block, is_last_line);
}

Err
_file_info_is_last_line_content(ContentBlockInfo *p_content, int block_offset, int line_offset, int n_block, bool *is_last_line)
{
    if(line_offset != p_content->n_line - 1) {
        *is_last_line = false;
        return S_OK;
    }

    if(block_offset == n_block - 1) {
        *is_last_line = true;
        return S_OK;
    }

    p_content++;
    for(int i = block_offset + 1; i < n_block; i++, p_content++) {
        if(p_content->n_line) {
            *is_last_line = false;
            return S_OK;
        }
    }

    *is_last_line = true;
    return S_OK;
}

Err
file_info_get_content_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, ContentBlockInfo **content_block)
{
    Err error_code = S_OK;
    CommentInfo *p_comment = NULL;
    switch(content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        *content_block = file_info->main_blocks + block_offset;
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        p_comment = file_info->comments + comment_offset;
        *content_block = p_comment->comment_reply_blocks + block_offset;
        break;
    default:
        *content_block = NULL;
        error_code = S_ERR;
        break;
    }

    return error_code;
}

Err
file_info_get_n_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int *n_block)
{
    CommentInfo *p_comment = NULL;
    switch(content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        *n_block = file_info->n_main_block;
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        p_comment = file_info->comments + comment_offset;
        *n_block = p_comment->n_comment_reply_block;
        break;
    default:
        *n_block = 0;
        break;
    }

    return S_OK;
}

Err
file_info_get_n_line(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, int *n_line)
{
    ContentBlockInfo *p_content = NULL;
    Err error_code = file_info_get_content_block(file_info, content_type, comment_offset, block_offset, &p_content);
    if(error_code) return error_code;

    *n_line = p_content->n_line;

    return S_OK;
}

Err
construct_file_info(UUID main_id, FileInfo *file_info)
{
    MainHeader main_header = {};
    Err error_code = read_main_header(main_id, &main_header);
    if(error_code) return error_code;

    memcpy(file_info->main_id, main_id, UUIDLEN);
    memcpy(file_info->main_poster, main_header.poster, IDLEN);
    file_info->main_create_milli_timestamp = main_header.create_milli_timestamp;
    file_info->main_update_milli_timestamp = main_header.update_milli_timestamp;
    memcpy(file_info->main_content_id, main_header.content_id, UUIDLEN);

    file_info->n_main_line = main_header.n_total_line;

    // main-blocks
    error_code = _get_file_info_set_content_block_info(file_info->main_content_id, main_header.n_total_block, file_info);

    // comment
    if(!error_code) {
        error_code = _get_file_info_set_comment_info(main_id, file_info);
    }

    int n_comment_reply_lines = 0;
    error_code = _get_file_info_total_comment_reply_lines(file_info, &n_comment_reply_lines);
    file_info->n_total_line = file_info->n_main_line + file_info->n_comment + n_comment_reply_lines;

    return error_code;
}

Err
_get_file_info_total_comment_reply_lines(FileInfo *file_info, int *n_comment_reply_lines)
{
    int tmp_n = 0;
    int n_comment = file_info->n_comment;
    CommentInfo *p_comment = file_info->comments;
    for(int i = 0; i < n_comment; i++, p_comment++) tmp_n += p_comment->n_comment_reply_total_line;

    *n_comment_reply_lines = tmp_n;

    return S_OK;
}

Err
_get_file_info_set_content_block_info(UUID main_content_id, int n_content_block, FileInfo *file_info)
{
    Err error_code = S_OK;

    file_info->n_main_block = n_content_block;
    file_info->main_blocks = malloc(sizeof(ContentBlockInfo) * n_content_block);
    bzero(file_info->main_blocks, sizeof(ContentBlockInfo) * n_content_block);

    bson_t **b_content_blocks = malloc(sizeof(bson_t *) * n_content_block);
    if(!b_content_blocks) error_code = S_ERR_MALLOC;

    bson_t *fields = BCON_NEW(
        "block_id", BCON_BOOL(true),
        "n_line", BCON_BOOL(true)
        );

    int tmp_n_content_block = 0;

    error_code = read_content_blocks_to_bsons(main_content_id, fields, n_content_block, MONGO_MAIN_CONTENT, b_content_blocks, &tmp_n_content_block);
    if(tmp_n_content_block != n_content_block) error_code = S_ERR;

    int n_line = 0;
    int block_id = 0;
    if(!error_code) {
        for(int i = 0; i < n_content_block; i++) {
            error_code = bson_get_value_int32(b_content_blocks[i], "n_line", &n_line);
            if(error_code) break;
            error_code = bson_get_value_int32(b_content_blocks[i], "block_id", &block_id);
            if(error_code) break;

            file_info->main_blocks[block_id].n_line = n_line;
            file_info->main_blocks[block_id].n_line_in_db = n_line;
            file_info->main_blocks[block_id].storage_type = PTTDB_STORAGE_TYPE_MONGO;
        }
    }

    // free
    safe_free_b_list(&b_content_blocks, n_content_block);
    bson_safe_destroy(&fields);

    return error_code;
}

Err
_get_file_info_set_comment_info(UUID main_id, FileInfo *file_info)
{
    UUID comment_id = {};
    time64_t create_milli_timestamp = 0;
    char poster[IDLEN + 1] = {};
    int max_n_comment = 0;
    int n_comment = 0;
    int n_read_comment = 0;
    bson_t **b_comments = NULL;

    bson_t *fields = BCON_NEW(
        "_id", BCON_BOOL(false),
        "the_id", BCON_BOOL(true),
        "poster", BCON_BOOL(true),
        "create_milli_timestamp", BCON_BOOL(true),
        "comment_reply_id", BCON_BOOL(true),
        "n_comment_reply_total_line", BCON_BOOL(true),
        "n_comment_reply_block", BCON_BOOL(true)
        );

    Err error_code = get_newest_comment(main_id, comment_id, &create_milli_timestamp, poster, &max_n_comment);
    if(error_code == S_ERR_NOT_EXISTS && !max_n_comment) error_code = S_OK;

    if(!error_code && max_n_comment) {
        b_comments = malloc(sizeof(bson_t *) * max_n_comment);
        if(!b_comments) error_code = S_ERR_MALLOC;
    }
    if(!error_code && max_n_comment) {
        bzero(b_comments, sizeof(bson_t *) * max_n_comment);
    }

    if(!error_code && max_n_comment) {
        error_code = read_comments_until_newest_to_bsons(main_id, create_milli_timestamp, poster, fields, max_n_comment, b_comments, &n_comment);
    }

    if(!error_code && max_n_comment) {
        error_code = ensure_b_comments_order(b_comments, n_comment, READ_COMMENTS_ORDER_TYPE_ASC);
    }

    if(!error_code && max_n_comment) {
        file_info->comments = malloc(sizeof(CommentInfo) * n_comment);
        if(!file_info->comments) error_code = S_ERR_MALLOC;
    }
    if(!error_code && max_n_comment){
        bzero(file_info->comments, sizeof(CommentInfo) * n_comment);
    }

    bson_t **p_b_comments = NULL;
    CommentInfo *p_comments = NULL;
    int end_i = 0;
    int each_n_comment = 0;
    if(!error_code && max_n_comment) {
        p_b_comments = b_comments;
        p_comments = file_info->comments;
        for(int i = 0; i < n_comment; i += N_FILE_INFO_SET_COMMENT_INFO_BLOCK, p_b_comments += N_FILE_INFO_SET_COMMENT_INFO_BLOCK, p_comments += N_FILE_INFO_SET_COMMENT_INFO_BLOCK) {
            end_i = n_comment < (i + N_FILE_INFO_SET_COMMENT_INFO_BLOCK) ? n_comment : (i + N_FILE_INFO_SET_COMMENT_INFO_BLOCK);
            each_n_comment = end_i - i;

            error_code = _get_file_info_set_comment_to_comment_info(p_b_comments, each_n_comment, p_comments, &n_read_comment);
            if(error_code) break;

            error_code = _get_file_info_set_comment_replys_to_comment_info(p_b_comments, each_n_comment, p_comments);
            if(error_code) break;
        }
    }

    file_info->n_comment = n_read_comment;

    // free
    safe_free_b_list(&b_comments, n_comment);
    bson_safe_destroy(&fields);

    return error_code;
}

Err
_get_file_info_set_comment_to_comment_info(bson_t **b_comments, int n_comment, CommentInfo *comments, int *n_read_comment)
{
    Err error_code = S_OK;
    bson_t **p_b_comments = b_comments;
    CommentInfo *p_comments = comments;

    int len = 0;
    int each_n_comment_reply_block = 0;
    int i = 0;
    for(i = 0; i < n_comment; i++, p_b_comments++, p_comments++) {
        error_code = bson_get_value_bin(*p_b_comments, "the_id", UUIDLEN, (char *)p_comments->comment_id, &len);
        if(error_code) break;

        error_code = bson_get_value_bin(*p_b_comments, "poster", IDLEN, p_comments->comment_poster, &len);
        if(error_code) break;

        error_code = bson_get_value_int64(*p_b_comments, "create_milli_timestamp", &p_comments->comment_create_milli_timestamp);
        if(error_code) break;

        error_code = bson_get_value_bin(*p_b_comments, "comment_reply_id", UUIDLEN, (char *)p_comments->comment_reply_id, &len);
        if(error_code) break;

        if(!memcmp(p_comments->comment_reply_id, EMPTY_ID, UUIDLEN)) continue;

        error_code = bson_get_value_int32(*p_b_comments, "n_comment_reply_total_line", &p_comments->n_comment_reply_total_line);
        if(error_code) break;

        error_code = bson_get_value_int32(*p_b_comments, "n_comment_reply_block", &each_n_comment_reply_block);
        if(error_code) break;

        p_comments->n_comment_reply_block = each_n_comment_reply_block;

        p_comments->comment_reply_blocks = malloc(sizeof(ContentBlockInfo) * each_n_comment_reply_block);
        if(!p_comments->comment_reply_blocks) {
            error_code = S_ERR_MALLOC;
            break;
        }
        bzero(p_comments->comment_reply_blocks, sizeof(ContentBlockInfo) * each_n_comment_reply_block);
    }

    *n_read_comment = i;

    return error_code;
}

Err
_get_file_info_set_comment_replys_to_comment_info(bson_t **b_comments, int n_comment, CommentInfo *comments)
{
    Err error_code = S_OK;
    bson_t *q_b_comment_reply_ids = NULL;
    bson_t *query_comment_reply_block = NULL;
    bson_t *comment_reply_block_fields = NULL;
    bson_t **b_comment_reply_blocks = NULL;
    int n_expected_comment_reply = 0;
    int n_expected_comment_reply_block = 0;
    int n_comment_reply_block = 0;

    UUID comment_id = {};

    DictIdxByUU dict_idx_by_uu = {};

    bson_t **p_b_comments = NULL;

    int len = 0;
    // dict-idx-by-uu
    error_code = init_dict_idx_by_uu(&dict_idx_by_uu, n_comment);
    if(!error_code) {
        p_b_comments = b_comments;
        for(int i = 0; i < n_comment; i++, p_b_comments++) {
            error_code = bson_get_value_bin(*p_b_comments, "the_id", UUIDLEN, (char *)comment_id, &len);
            if(error_code) break;

            error_code = add_to_dict_idx_by_uu(comment_id, i, &dict_idx_by_uu);
            if(error_code) break;
        }
    }

    // comment-reply
    if(!error_code) {
        comment_reply_block_fields = BCON_NEW(
            "_id", BCON_BOOL(false),
            "the_id", BCON_BOOL(true),
            "ref_id", BCON_BOOL(true),
            "block_id", BCON_BOOL(true),
            "n_line", BCON_BOOL(true)
            );

        if (!comment_reply_block_fields) error_code = S_ERR_MALLOC;
    }

    if (!error_code) {
        error_code = extract_b_comments_comment_reply_id_to_bsons(b_comments, n_comment, "$in", &q_b_comment_reply_ids, &n_expected_comment_reply, &n_expected_comment_reply_block);
    }

    if (!error_code && n_expected_comment_reply) {
        query_comment_reply_block = BCON_NEW(
            "the_id", BCON_DOCUMENT(q_b_comment_reply_ids)
            );

        if (!query_comment_reply_block) error_code = S_ERR_MALLOC;
    }

    if(!error_code && n_expected_comment_reply) {
        b_comment_reply_blocks = malloc(sizeof(bson_t *) * n_expected_comment_reply_block);
        if(!b_comment_reply_blocks) error_code = S_ERR_MALLOC;
    }
    if(!error_code && n_expected_comment_reply) {
        bzero(b_comment_reply_blocks, sizeof(bson_t *) * n_expected_comment_reply_block);
    }

    if(!error_code && n_expected_comment_reply) {
        error_code = read_content_blocks_by_query_to_bsons(query_comment_reply_block, comment_reply_block_fields, n_expected_comment_reply_block, MONGO_COMMENT_REPLY_BLOCK, b_comment_reply_blocks, &n_comment_reply_block);
        if(n_expected_comment_reply_block != n_comment_reply_block) error_code = S_ERR;
    }

    bson_t **p_b_comment_reply_blocks = b_comment_reply_blocks;
    int each_n_line = 0;
    int each_block_id = 0;
    int each_idx = 0;
    UUID each_comment_reply_id = {};

    if(!error_code && n_expected_comment_reply) {
        for(int i = 0; i < n_comment_reply_block; i++, p_b_comment_reply_blocks++) {
            error_code = bson_get_value_bin(*p_b_comment_reply_blocks, "ref_id", UUIDLEN, (char *)each_comment_reply_id, &len);
            if(error_code) break;

            error_code = bson_get_value_int32(*p_b_comment_reply_blocks, "block_id", &each_block_id);
            if(error_code) break;

            error_code = bson_get_value_int32(*p_b_comment_reply_blocks, "n_line", &each_n_line);
            if(error_code) break;

            error_code = get_idx_from_dict_idx_by_uu(&dict_idx_by_uu, each_comment_reply_id, &each_idx);
            if(error_code) break;

            comments[each_idx].comment_reply_blocks[each_block_id].n_line = each_n_line;
            comments[each_idx].comment_reply_blocks[each_block_id].n_line_in_db = each_n_line;
        }
    }

    // free
    safe_destroy_dict_idx_by_uu(&dict_idx_by_uu);
    safe_free_b_list(&b_comment_reply_blocks, n_comment_reply_block);
    bson_safe_destroy(&comment_reply_block_fields);
    bson_safe_destroy(&query_comment_reply_block);
    bson_safe_destroy(&q_b_comment_reply_ids);

    return error_code;
}

Err
destroy_file_info(FileInfo *file_info)
{
    if(file_info->main_blocks) {
        free(file_info->main_blocks);
        file_info->main_blocks = NULL;
    }

    CommentInfo *p_comments = file_info->comments;
    int n_comment = file_info->n_comment;

    if(p_comments) {
        for(int i = 0; i < n_comment; i++, p_comments++) {
            if(!p_comments->comment_reply_blocks) continue;

            free(p_comments->comment_reply_blocks);
        }

        free(file_info->comments);
    }

    bzero(file_info, sizeof(FileInfo));

    return S_OK;
}

Err
file_info_get_pre_line(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type)
{
    if(file_info_is_first_line_ne(file_info, orig_content_type, orig_comment_offset, orig_block_offset, orig_line_offset)) return S_ERR_NOT_EXISTS;

    Err (*p_func)(FileInfo *, UUID, enum PttDBContentType, int, int, int, UUID, enum PttDBContentType *, int *, int *, int *, enum PttDBStorageType *) = NULL;

    switch (orig_content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        p_func = _file_info_get_pre_line_main;
        break;
    case PTTDB_CONTENT_TYPE_COMMENT:
        p_func = _file_info_get_pre_line_comment;
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        p_func = _file_info_get_pre_line_comment_reply;
        break;
    default:
        break;
    }
    if (!p_func) return S_ERR;

    Err error_code = p_func(file_info, orig_id, orig_content_type, orig_block_offset, orig_line_offset, orig_comment_offset, new_id, new_content_type, new_block_offset, new_line_offset, new_comment_offset, new_storage_type);

    return error_code;
}

Err
_file_info_get_pre_line_main(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset GCC_UNUSED, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type)
{
    memcpy(new_id, orig_id, UUIDLEN);
    *new_content_type = orig_content_type;
    *new_comment_offset = 0;

    ContentBlockInfo *p_content_block = file_info->main_blocks + orig_block_offset;

    fprintf(stderr, "file_info._file_info_get_pre_line_main: orig: (content_type: %d comment: %d block: %d line: %d)\n", orig_content_type, orig_comment_offset, orig_block_offset, orig_line_offset);

    Err error_code = S_OK;

    if (orig_line_offset) {
        *new_block_offset = orig_block_offset;
        *new_line_offset = orig_line_offset - 1;
        *new_storage_type = p_content_block->storage_type;
    }
    else {
        error_code = file_info_pre_block(file_info, orig_content_type, orig_comment_offset, orig_block_offset, new_block_offset);
        fprintf(stderr, "file_info._file_info_get_pre_line_main: after pre_block: orig: (content_type: %d comment: %d block: %d new_block: %d e: %d\n", orig_content_type, orig_comment_offset, orig_block_offset, *new_block_offset, error_code);
        if(error_code) return error_code;

        p_content_block = file_info->main_blocks + *new_block_offset;

        *new_line_offset = p_content_block->n_line - 1;
        *new_storage_type = p_content_block->storage_type;
    }

    return error_code;
}

Err
_file_info_get_pre_line_comment(FileInfo *file_info, UUID orig_id GCC_UNUSED, enum PttDBContentType orig_content_type GCC_UNUSED, int orig_block_offset GCC_UNUSED, int orig_line_offset GCC_UNUSED, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type)
{
    Err error_code = S_OK;
    CommentInfo *p_comment = NULL;
    ContentBlockInfo *p_content_block = NULL;

    if (orig_comment_offset == 0) {
        memcpy(new_id, file_info->main_content_id, UUIDLEN);
        *new_content_type = PTTDB_CONTENT_TYPE_MAIN;

        error_code = file_info_last_block(file_info, PTTDB_CONTENT_TYPE_MAIN, 0, new_block_offset);

        p_content_block = file_info->main_blocks + *new_block_offset;

        *new_line_offset = p_content_block->n_line - 1;
        *new_comment_offset = 0;
        *new_storage_type = p_content_block->storage_type;
    }
    else {
        *new_comment_offset = orig_comment_offset - 1;
        p_comment = file_info->comments + *new_comment_offset;

        if (p_comment->n_comment_reply_total_line) {
            memcpy(new_id, p_comment->comment_reply_id, UUIDLEN);
            *new_content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
            error_code = file_info_last_block(file_info, PTTDB_CONTENT_TYPE_COMMENT_REPLY, *new_comment_offset, new_block_offset);
            p_content_block = p_comment->comment_reply_blocks + *new_block_offset;
            *new_line_offset = p_content_block->n_line - 1;
            *new_storage_type = p_content_block->storage_type;
        }
        else {
            memcpy(new_id, p_comment->comment_id, UUIDLEN);
            *new_content_type = PTTDB_CONTENT_TYPE_COMMENT;
            *new_block_offset = 0;
            *new_line_offset = 0;
            *new_storage_type = p_comment->storage_type;
        }
    }

    return S_OK;
}

Err
_file_info_get_pre_line_comment_reply(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type GCC_UNUSED, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type)
{
    Err error_code = S_OK;
    CommentInfo *p_comment = file_info->comments + orig_comment_offset;
    ContentBlockInfo *p_comment_reply_block = p_comment->comment_reply_blocks + orig_block_offset;

    *new_comment_offset = orig_comment_offset;
    if (file_info_is_first_block_ne(file_info, PTTDB_CONTENT_TYPE_COMMENT_REPLY, orig_comment_offset, orig_block_offset) && orig_line_offset == 0) {
        memcpy(new_id, p_comment->comment_id, UUIDLEN);
        *new_content_type = PTTDB_CONTENT_TYPE_COMMENT;
        *new_block_offset = 0;
        *new_line_offset = 0;
        *new_storage_type = p_comment->storage_type;
    }
    else {
        memcpy(new_id, orig_id, UUIDLEN);
        *new_content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;

        if (orig_line_offset != 0) {
            *new_block_offset = orig_block_offset;
            *new_line_offset = orig_line_offset - 1;
            *new_storage_type = p_comment_reply_block->storage_type;
        }
        else {
            error_code = file_info_pre_block(file_info, PTTDB_CONTENT_TYPE_COMMENT_REPLY, orig_comment_offset, orig_block_offset, new_block_offset);
            p_comment_reply_block =p_comment->comment_reply_blocks + *new_block_offset;

            *new_line_offset = p_comment_reply_block->n_line - 1;
            *new_storage_type = p_comment_reply_block->storage_type;
        }
    }

    return error_code;
}

Err
file_info_get_next_line(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type)
{
    Err error_code = S_OK;

    Err (*p_func)(FileInfo *, UUID, enum PttDBContentType, int, int, int, UUID, enum PttDBContentType *, int *, int *, int *, enum PttDBStorageType *) = NULL;

    switch (orig_content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        p_func = _file_info_get_next_line_main;
        break;
    case PTTDB_CONTENT_TYPE_COMMENT:
        p_func = _file_info_get_next_line_comment;
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        p_func = _file_info_get_next_line_comment_reply;
        break;
    default:
        break;
    }
    if (!p_func) return S_ERR;

    error_code = p_func(file_info, orig_id, orig_content_type, orig_block_offset, orig_line_offset, orig_comment_offset, new_id, new_content_type, new_block_offset, new_line_offset, new_comment_offset, new_storage_type);

    return error_code;
}

Err
_file_info_get_next_line_main(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset GCC_UNUSED, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type)
{
    ContentBlockInfo *p_content_block = file_info->main_blocks + orig_block_offset;

    // last line of main-block. new-buffer as comment
    bool is_last_block = false;
    Err error_code = file_info_is_last_block(file_info, orig_content_type, orig_comment_offset, orig_block_offset, &is_last_block);

    CommentInfo *p_comment = NULL;
    if (is_last_block && orig_line_offset == p_content_block->n_line - 1) {
        p_comment = file_info->comments;
        memcpy(new_id, p_comment->comment_id, UUIDLEN);
        *new_content_type = PTTDB_CONTENT_TYPE_COMMENT;
        *new_comment_offset = 0;
        *new_block_offset = 0;
        *new_line_offset = 0;
        *new_storage_type = p_comment->storage_type;

        return S_OK;
    }

    // still in main-block
    memcpy(new_id, orig_id, UUIDLEN);
    *new_content_type = orig_content_type;
    *new_comment_offset = 0;

    if (orig_line_offset != p_content_block->n_line - 1) {
        // not the last-line
        *new_block_offset = orig_block_offset;
        *new_line_offset = orig_line_offset + 1;
        *new_storage_type = p_content_block->storage_type;
    }
    else {
        // last-line, but not the last block
        error_code = file_info_next_block(file_info, *new_content_type, *new_comment_offset, orig_block_offset, new_block_offset);

        p_content_block = file_info->main_blocks + *new_block_offset;

        *new_line_offset = 0;
        *new_storage_type = p_content_block->storage_type;
    }

    return S_OK;
}

Err
_file_info_get_next_line_comment(FileInfo *file_info, UUID orig_id GCC_UNUSED, enum PttDBContentType orig_content_type GCC_UNUSED, int orig_block_offset GCC_UNUSED, int orig_line_offset GCC_UNUSED, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type)
{
    Err error_code = S_OK;
    CommentInfo *p_comment = file_info->comments + orig_comment_offset;
    ContentBlockInfo *p_comment_reply_block = NULL;

    if (p_comment->n_comment_reply_total_line) {
        // with comment-reply
        memcpy(new_id, p_comment->comment_reply_id, UUIDLEN);
        *new_content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
        *new_comment_offset = orig_comment_offset;

        error_code = file_info_first_block(file_info, PTTDB_CONTENT_TYPE_COMMENT_REPLY, orig_comment_offset, new_block_offset);

        p_comment_reply_block = p_comment->comment_reply_blocks + *new_block_offset;

        *new_line_offset = 0;
        *new_storage_type = p_comment_reply_block->storage_type;
    }
    else {
        // next comment
        p_comment++;

        memcpy(new_id, p_comment->comment_id, UUIDLEN);
        *new_content_type = PTTDB_CONTENT_TYPE_COMMENT;
        *new_comment_offset = orig_comment_offset + 1;
        *new_block_offset = 0;
        *new_line_offset = 0;
        *new_storage_type = p_comment->storage_type;
    }

    return S_OK;
}

Err
_file_info_get_next_line_comment_reply(FileInfo *file_info, UUID orig_id GCC_UNUSED, enum PttDBContentType attr GCC_UNUSED, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type)
{
    Err error_code = S_OK;

    CommentInfo *p_comment = file_info->comments + orig_comment_offset;
    ContentBlockInfo *p_comment_reply_block = p_comment->comment_reply_blocks + orig_block_offset;

    if (orig_line_offset != p_comment_reply_block->n_line - 1) {
        // within the same block
        memcpy(new_id, p_comment->comment_reply_id, UUIDLEN);
        *new_content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
        *new_comment_offset = orig_comment_offset;
        *new_block_offset = orig_block_offset;
        *new_line_offset = orig_line_offset + 1;
        *new_storage_type = p_comment_reply_block->storage_type;
    }
    else if (!file_info_is_last_block_ne(file_info, PTTDB_CONTENT_TYPE_COMMENT_REPLY, orig_comment_offset, orig_block_offset)) {
        // different block, within same comment
        memcpy(new_id, p_comment->comment_reply_id, UUIDLEN);
        *new_content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;

        *new_comment_offset = orig_comment_offset;

        error_code = file_info_next_block(file_info, PTTDB_CONTENT_TYPE_COMMENT_REPLY, orig_comment_offset, orig_block_offset, new_block_offset);

        p_comment_reply_block = p_comment->comment_reply_blocks + *new_block_offset;

        *new_line_offset = 0;

        *new_storage_type = p_comment_reply_block->storage_type;
    }
    else {
        // different comment
        p_comment++;
        memcpy(new_id, p_comment->comment_id, UUIDLEN);

        *new_content_type = PTTDB_CONTENT_TYPE_COMMENT;
        *new_comment_offset = orig_comment_offset + 1;
        *new_block_offset = 0;
        *new_line_offset = 0;

        *new_storage_type = p_comment->storage_type;
    }
    return S_OK;
}

/**********
 * save to db
 **********/
Err
save_file_info_to_db(FileInfo *file_info, char *user, char *ip)
{
    Err error_code = _save_file_info_to_db_main(file_info, user, ip);
    if(error_code) return error_code;

    error_code = _save_file_info_to_db_comment(file_info, user, ip);
    if(error_code) return error_code;

    error_code = _save_file_info_to_db_comment_reply(file_info, user, ip);
    if(error_code) return error_code;

    return S_OK;
}

Err
_save_file_info_to_db_main(FileInfo *file_info, char *user, char *ip)
{
    bool is_modified = false;
    Err error_code = _save_file_info_to_db_is_modified(file_info->main_blocks, file_info->n_main_block, &is_modified);
    if(error_code) return error_code;
    if(!is_modified) return S_OK;

    UUID content_id = {};
    error_code = update_main_from_content_block_infos(
        file_info->main_id,
        user,
        ip,
        file_info->main_content_id,
        file_info->n_main_block,
        file_info->main_blocks,
        content_id,
        0);

    return error_code;
}

Err
_save_file_info_to_db_comment(FileInfo *file_info, char *user, char *ip)
{
    Err error_code = S_OK;

    CommentInfo *p_comment = file_info->comments;
    for(int i = 0; i < file_info->n_comment; i++, p_comment++) {
        if(p_comment->storage_type != PTTDB_STORAGE_TYPE_FILE) continue;

        error_code = update_comment_from_comment_info(file_info->main_id, p_comment, user, ip, 0);
    }
    return S_OK;
}

Err
_save_file_info_to_db_comment_reply(FileInfo *file_info, char *user, char *ip)
{
    Err error_code = S_OK;
    CommentInfo *p_comment = file_info->comments;
    ContentBlockInfo *p_content_blocks = NULL;
    bool is_modified = false;

    log_file_info(file_info, "pttdb_file_info._save_file_info_to_db_comment_reply");

    UUID new_comment_reply_id = {};
    bool is_all_whitespace = false;
    for(int i = 0; i < file_info->n_comment; i++, p_comment++) {
        p_content_blocks = p_comment->comment_reply_blocks;
        error_code = _save_file_info_to_db_is_modified(p_content_blocks, p_comment->n_comment_reply_block, &is_modified);
        if(error_code) break;
        if(!is_modified) continue;

        fprintf(stderr, "pttdb_file_info._save_file_info_to_db_comment_reply: (%d/%d): is-modified\n", i, file_info->n_comment);

        error_code = _save_file_info_to_db_comment_reply_is_all_whitespace(file_info->main_id, p_comment->comment_reply_id, p_comment->n_comment_reply_block, p_comment->comment_reply_blocks, &is_all_whitespace);

        if(is_all_whitespace) {
            error_code = update_comment_reply_to_comment(p_comment->comment_id, (unsigned char *)EMPTY_ID, 0, 0, 0);
            continue;
        }

        bzero(new_comment_reply_id, sizeof(UUID));
        error_code = create_comment_reply_from_content_block_infos(
            file_info->main_id,
            p_comment->comment_id,
            user,
            ip,
            p_comment->comment_reply_id,
            p_comment->n_comment_reply_block,
            p_content_blocks,
            new_comment_reply_id,
            0);

        if(error_code) break;

    }
    return S_OK;
}

Err
_save_file_info_to_db_comment_reply_is_all_whitespace(UUID main_id, UUID comment_reply_id, int n_comment_reply_block, ContentBlockInfo *comment_reply_blocks, bool *is_all_whitespace)
{
    Err error_code = S_OK;
    ContentBlockInfo *p_content_block = comment_reply_blocks;

    char *buf = NULL;
    int len = 0;
    bool is_end = false;
    *is_all_whitespace = true;
    int n_file = 0;
    char *p_buf = NULL;
    for(int i = 0; i < n_comment_reply_block; i++, p_content_block++) {
        n_file = p_content_block->n_file;
        for(int j = 0; j < n_file; j++) {
            error_code = pttdb_file_get_data(main_id, PTTDB_CONTENT_TYPE_COMMENT_REPLY, comment_reply_id, i, j, &buf, &len);
            p_buf = buf;
            for(int k = 0; k < len; k++, p_buf++) {
                if(*p_buf != '\n' && *p_buf != '\r' && *p_buf != ' ') {
                    *is_all_whitespace = false;
                    is_end = true;
                    break;
                }
            }
            safe_free((void **)&buf);
            if(is_end) break;
        }
        if(is_end) break;
    }

    return S_OK;
}

Err
_save_file_info_to_db_is_modified(ContentBlockInfo *content_blocks, int n_content_block, bool *is_modified)
{
    ContentBlockInfo *p_content_block = content_blocks;
    int i = 0;
    for(i = 0; i < n_content_block; i++, p_content_block++) {
        if(p_content_block->storage_type == PTTDB_STORAGE_TYPE_FILE) break;
    }
    *is_modified = i == n_content_block ? false : true;

    return S_OK;
}

Err
log_file_info(FileInfo *file_info, char *prompt)
{
    ContentBlockInfo *p_content_block = file_info->main_blocks;
    for(int i = 0; i < file_info->n_main_block; i++, p_content_block++) {
        fprintf(stderr, "%s: main-block: (%d/%d): (n-line: %d n-line-in-db: %d n-new-line: %d n-to-delete-line: %d storage-type: %d, n-file: %d\n", prompt, i, file_info->n_main_block, p_content_block->n_line, p_content_block->n_line_in_db, p_content_block->n_new_line, p_content_block->n_to_delete_line, p_content_block->storage_type, p_content_block->n_file);
        for(int j = 0; j < p_content_block->n_file; j++) {
            fprintf(stderr, "%s: main-block: file: (%d/%d.%d/%d): %d\n", prompt, i, file_info->n_main_block, j, p_content_block->n_file, p_content_block->file_n_line[j]);
        }
    }

    CommentInfo *p_comment = file_info->comments;
    for(int k = 0; k < file_info->n_comment; k++, p_comment++) {
        fprintf(stderr, "%s: comment: (%d/%d) n_comment_reply_block: %d n_comment_reply_total_line: %d\n", prompt, k, file_info->n_comment, p_comment->n_comment_reply_block, p_comment->n_comment_reply_total_line);
        p_content_block = p_comment->comment_reply_blocks;

        for(int i = 0; i < p_comment->n_comment_reply_block; i++, p_content_block++) {
            fprintf(stderr, "%s: comment-reply-block: (%d%d.%d/%d): (n-line: %d n-line-in-db: %d n-new-line: %d n-to-delete-line: %d storage-type: %d, n-file: %d\n", prompt, k, file_info->n_comment, i, p_comment->n_comment_reply_block, p_content_block->n_line, p_content_block->n_line_in_db, p_content_block->n_new_line, p_content_block->n_to_delete_line, p_content_block->storage_type, p_content_block->n_file);
            for(int j = 0; j < p_content_block->n_file; j++) {
                fprintf(stderr, "%s: comment-reply-block: file: (%d/%d.%d/%d.%d/%d): %d\n", prompt, k, file_info->n_comment, i, p_comment->n_comment_reply_block, j, p_content_block->n_file, p_content_block->file_n_line[j]);
            }
        }

    }
    fprintf(stderr, "%s: n-total-line: %d\n", prompt, file_info->n_total_line);

    return S_OK;
}

Err
file_info_increase_main_content_line(FileInfo *file_info, int block_offset, int file_offset)
{
    if(block_offset >= file_info->n_main_block) return S_ERR;

    ContentBlockInfo *p_content = file_info->main_blocks + block_offset;

    Err error_code = _file_info_increase_content_line_core(p_content, file_offset);
    if(error_code) return error_code;

    file_info->n_main_line++;
    file_info->n_total_line++;

    return S_OK;
}

Err
file_info_increase_comment_reply_line(FileInfo *file_info, int comment_offset, int block_offset, int file_offset)
{
    if(comment_offset >= file_info->n_comment) return S_ERR;

    CommentInfo *p_comment = file_info->comments + comment_offset;
    if(block_offset >= p_comment->n_comment_reply_block) return S_ERR;

    ContentBlockInfo *p_content = p_comment->comment_reply_blocks + block_offset;

    Err error_code = _file_info_increase_content_line_core(p_content, file_offset);
    if(error_code) return error_code;

    p_comment->n_comment_reply_total_line++;
    file_info->n_total_line++;

    return S_OK;
}

Err
file_info_update_main_content_storage_type(FileInfo *file_info, int block_offset, enum PttDBStorageType storage_type)
{
    if(block_offset >= file_info->n_main_block) return S_ERR;

    ContentBlockInfo *p_content = file_info->main_blocks + block_offset;

    p_content->storage_type = storage_type;

    return S_OK;

}

Err
_file_info_increase_content_line_core(ContentBlockInfo *content_block, int file_offset)
{
    if(content_block->storage_type == PTTDB_STORAGE_TYPE_FILE && file_offset >= content_block->n_file) return S_ERR;

    content_block->n_line++;
    content_block->n_new_line++;

    if(content_block->storage_type == PTTDB_STORAGE_TYPE_FILE) {
        content_block->file_n_line[file_offset]++;
    }

    return S_OK;
}

Err
file_info_update_comment_storage_type(FileInfo *file_info, int comment_offset, enum PttDBStorageType storage_type)
{
    if(comment_offset == file_info->n_comment) return S_ERR;

    CommentInfo *p_comment = file_info->comments + comment_offset;
    p_comment->storage_type = storage_type;

    return S_OK;
}

Err
file_info_update_comment_reply_storage_type(FileInfo *file_info, int comment_offset, int block_offset, enum PttDBStorageType storage_type)
{
    if(comment_offset >= file_info->n_comment) return S_ERR;

    CommentInfo *p_comment = file_info->comments + comment_offset;
    if(block_offset >= p_comment->n_comment_reply_block) return S_ERR;

    ContentBlockInfo *p_content = p_comment->comment_reply_blocks + block_offset;
    p_content->storage_type = storage_type;

    return S_OK;
}

Err
file_info_decrease_main_content_line(FileInfo *file_info, int block_offset, int file_offset)
{

   if(block_offset >= file_info->n_main_block) return S_ERR;

    ContentBlockInfo *p_content = file_info->main_blocks + block_offset;

    Err error_code = _file_info_decrease_content_line_core(p_content, file_offset);
    if(error_code) return error_code;

    file_info->n_main_line--;
    file_info->n_total_line--;

    return S_OK;
}

Err
file_info_decrease_comment_reply_line(FileInfo *file_info, int comment_offset, int block_offset, int file_offset)
{
    if(comment_offset >= file_info->n_comment) return S_ERR;

    CommentInfo *p_comment = file_info->comments + comment_offset;
    if(block_offset >= p_comment->n_comment_reply_block) return S_ERR;

    ContentBlockInfo *p_content = p_comment->comment_reply_blocks + block_offset;

    Err error_code = _file_info_decrease_content_line_core(p_content, file_offset);
    if(error_code) return error_code;

    p_comment->n_comment_reply_total_line--;
    file_info->n_total_line--;

    return S_OK;
}

Err
_file_info_decrease_content_line_core(ContentBlockInfo *content_block, int file_offset)
{
    if(content_block->storage_type == PTTDB_STORAGE_TYPE_FILE && file_offset >= content_block->n_file) return S_ERR;

    content_block->n_line--;
    content_block->n_to_delete_line++;

    if(content_block->storage_type == PTTDB_STORAGE_TYPE_FILE) {
        content_block->file_n_line[file_offset]--;
    }

    return S_OK;
}
