#include "pttdb.h"
#include "pttdb_internal.h"

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param buf [description]
 * @param bytes [description]
 * @param ref_id [description]
 * @param conten_id [description]
 * @param MongoDBId [description]
 * @param n_line [description]
 * @param n_block [description]
 */
Err
split_contents(char *buf, int bytes, UUID ref_id, UUID conten_id, enum MongoDBId mongo_db_id, int *n_line, int *n_block) {
    Err error_code = S_OK;
    int bytes_in_line = 0;
    char line[MAX_BUF_SIZE] = {};
    ContentBlock content_block = {};

    // init
    *n_block = 0;
    error_code = _init_content_block(&content_block, ref_id, content_id, n_block);
    if(error_code) return error_code;

    error_code = _split_contents_core(buf, bytes, ref_id, content_id, mongo_db_id, n_line, n_block, line, &bytes_in_line, &content_block);
    if(error_code) return error_code;

    error_code = _split_contents_deal_with_last_line_block(bytes_in_line, line, ref_id, content_id, mongo_db_id, content_block, n_line, n_block);
    if(error_code) return error_code;

    return S_OK;
}

Err
split_contents_from_fd(int fd_content, int len, UUID ref_id, UUID content_id, int *line, int *n_block) {
    Err error_code = S_OK;
    char buf[MAX_BUF_SIZE] = {};
    char line[MAX_BUF_SIZE] = {};
    int bytes = 0;
    int buf_size = 0;
    int bytes_in_line = 0;
    int bytes_in_new_line = 0;
    ContentBlock content_block = {};

    // init
    *n_block = 0;
    error_code = _init_content_block(&content_block, ref_id, content_id, n_block);
    if(error_code) return error_code;

    // while-loop
    while ((buf_size = len < MAX_BUF_SIZE ? len : MAX_BUF_SIZE) && (bytes = read(fd_content, buf, buf_size)) > 0) {
        error_code = _split_contents_core(buf, bytes, ref_id, content_id, mongo_db_id, n_line, n_block, line, &bytes_in_line, &content_block);
        if(error_code) return error_code;

        len -= bytes;
    }

    error_code = _split_contents_deal_with_last_line_block(bytes_in_line, line, ref_id, content_id, mongo_db_id, content_block, n_line, n_block);
    if(error_code) return error_code;

    return S_OK;
}

Err
delete_content(UUID content_id, enum MongoDBId mongo_db_id)
{
    Err error_code = S_OK;
    bool bson_status;
    bson_t key;

    bson_init(&key);

    bson_status = bson_append_bin(&key, "the_id", -1, content_id, UUIDLEN);
    if(!bson_status) error_code = S_ERR;

    if(error_code == S_OK) {
        error_code = db_remove(mongo_db_id, &key);
    }

    bson_destroy(&key);

    return error_code;
}



/**
 * @brief core for split contents.
 * @details core for split contents. No need to deal with last line block if used by split_contents_from_fd
 *          (There are multiple input-bufs in split_contents_from_fd and split_contents_from_fd will take care of last line-block)
 * 
 * @param buf [description]
 * @param bytes [description]
 * @param ref_id [description]
 * @param content_id [description]
 * @param MongoDBId [description]
 * @param n_line [description]
 * @param n_block [description]
 * @param is_deal_wtih_last_line_block [description]
 */
Err
_split_contents_core(char *buf, int bytes, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, int *n_line, int *n_block, char *line, int *bytes_in_line, ContentBlock *content_block)
{
    Err error_code;

    int bytes_in_new_line = 0;
    for (int offset_buf = 0; offset_buf < bytes; offset_buf += bytes_in_new_line) {
        error_code = get_line_from_buf(buf, offset_buf, bytes, line, *bytes_in_line, &bytes_in_new_line);
        *bytes_in_line += bytes_in_new_line;
        // unable to get more lines from buf
        if (error_code) break;

        // Main-op
        error_code = _split_contents_core_core(line, *bytes_in_line, ref_id, content_id, content, n_line, n_block);
        if (error_code) return error_code;

        // reset line
        bzero(line, sizeof(char) * *bytes_in_line);
        *bytes_in_line = 0;
    }

    return S_OK;
}

Err
_split_contents_core_core(char *line, int bytes_in_line, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, ContentBlock *content_block, int *n_line, int *n_block)
{
    Err error_code;

    // check for max-lines in block-buf
    if(content_block->n_line  > MAX_BUF_LINES) {
        error_code = _save_content_block(content_block, mongo_db_id);
        if(error_code) return error_code;

        error_code = _init_content_block(content_block, ref_id, content_id, n_block);
        if(error_code) return error_code;
    }
    // check for max-buf in block-buf
    else if (content_block->len_block + bytes_in_line > MAX_BUF_BLOCK) {
        error_code = =_save_content_block(content_block, mongo_db_id);
        if(error_code) return error_code;

        error_code = _init_content_block(content_block, ref_id, content_id, n_block);
        if(error_code) return error_code;
    }

    memcpy(content_block->buf_block + content_block->len_block, line, bytes_in_line);
    content_block->len_block += bytes_in_line;

    // 1 more line
    if(line[bytes_in_line - 2] == '\r' && line[bytes_in_line - 1] == '\n') {
        (*n_line)++;
        content_block->n_line++;
    }

    return S_OK;
}

/**
 * @brief [brief description]
 * @details initialize new content-block with block-id as current n_block.
 * 
 * @param content_block [description]
 * @param ref_id [description]
 * @param content_id [description]
 * @param n_block [description]
 */
Err
_init_content_block(ContentBlock *content_block, UUID ref_id, UUID content_id, int *n_block)
{
    if(content_block->len_block) {
        bzero(content_block->buf_block, content_block->len_block);
    }

    content_block->len_block = 0;
    content_block->n_line = 0;
    memcpy(content_block->the_id, content_id, sizeof(UUID));
    memcpy(content_block->ref_id, content_id, sizeof(UUID));

    content_block->block_id = *n_block;
    (*n_block)++;

    return S_OK;
}

Err
_save_content_block(ContentBlock *content_block, enum MongoDBId mongo_db_id)
{
    Error error_code;
    bson_t content_block_bson;
    bson_t content_block_id_bson;

    bson_init(&content_block_bson);
    bson_init(&content_block_id_bson);

    error_code = _serialize_content_block_bson(content_block, &content_block_bson);

    if(error_code == S_OK) {
        error_code = _serialize_content_id_bson(content->the_id, MONGO_THE_ID, content->block_id, &content_id_bson);
    }
    
    if(error_code == S_OK) {
        error_code = db_update_one(mongo_db_id, &content_id_bson, &content_bson, true);
    }

    bson_destroy(&content_bson);
    bson_destroy(&content_id_bson);

    return error_code;
}

Err
_split_contents_deal_with_last_line_block(int bytes_in_line, char *line, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, ContentBlock *content_block, int *n_line, int *n_block)
{
    // last line
    if (bytes_in_line) {
        // Main-op
        error_code = _split_contents_core_core(line, bytes_in_line, ref_id, content_id, content_block, n_line, n_block);
        if (error_code) return error_code;
    }

    // last block
    if (content_block.len_block) {
        error_code = _save_content_block(content, mongo_db_id);
        if (error_code) return error_code;
    }

    return S_OK;
}

/**
 * @brief Serialize main-content-block to bson
 * @details Serialize main-content-block to bson
 *
 * @param main_content_block main-content-block
 * @param main_content_block_bson main_content_block_bson (to-compute)
 * @return Err
 */
Err
_serialize_content_block_bson(ContentBlock *content_block, bson_t *content_block_bson)
{
    bool bson_status;

    bson_status = bson_append_bin(content_block_bson, "the_id", -1, content_block->the_id, UUIDLEN);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_bin(content_block_bson, "id", -1, content_block->id, UUIDLEN);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_int32(content_block_bson, "block_id", -1, content_block->block_id);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_int32(content_block_bson, "len_block", -1, content_block->len_block);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_int32(content_block_bson, "n_line", -1, content_block->n_line);
    if (!bson_status) return S_ERR;

    bson_status = bson_append_bin(content_block_bson, "buf_block", -1, content_block->buf_block, content_block->len_block);
    if (!bson_status) return S_ERR;

    return S_OK;
}

/**
 * @brief Deserialize bson to main-content-block
 * @details [long description]
 *
 * @param main_content_block_bson [description]
 * @param main_content_block [description]
 *
 * @return [description]
 */
Err
_deserialize_content_block_bson(bson_t *content_block_bson, ContentBlock *content_block)
{
    Err error_code;

    int len;
    error_code = bson_get_value_bin(content_block_bson, "the_id", UUIDLEN, content_block->the_id, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(content_block_bson, "id", UUIDLEN, content_block->id, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(content_block_bson, "block_id", &content_block->block_id);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(content_block_bson, "len_block", &content_block->len_block);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(content_block_bson, "n_line", &content_block->n_line);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(content_block_bson, "buf_block", current_block->len_block, content_block->buf_block, &len);
    if (error_code) return error_code;

    return S_OK;
}
