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
split_contents(char *buf, int bytes, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, int *n_line, int *n_block) {
    Err error_code = S_OK;
    int bytes_in_line = 0;
    char line[MAX_BUF_SIZE] = {};

    ContentBlock content_block = {};

    // init
    error_code = init_content_block_with_buf_block(&content_block, ref_id, content_id, *n_block);
    (*n_block)++;

    if (!error_code) {
        error_code = _split_contents_core(buf, bytes, ref_id, content_id, mongo_db_id, n_line, n_block, line, &bytes_in_line, &content_block);
    }

    if (!error_code) {
        error_code = _split_contents_deal_with_last_line_block(bytes_in_line, line, ref_id, content_id, mongo_db_id, &content_block, n_line, n_block);
    }

    destroy_content_block(&content_block);

    return error_code;
}

Err
split_contents_from_fd(int fd_content, int len, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, int *n_line, int *n_block) {
    Err error_code = S_OK;
    char buf[MAX_BUF_SIZE] = {};
    char line[MAX_BUF_SIZE] = {};
    int bytes = 0;
    int buf_size = 0;
    int bytes_in_line = 0;
    ContentBlock content_block = {};

    // init
    *n_block = 0;
    error_code = init_content_block_with_buf_block(&content_block, ref_id, content_id, *n_block);
    (*n_block)++;

    if (!error_code) {
        while ((buf_size = len < MAX_BUF_SIZE ? len : MAX_BUF_SIZE) && (bytes = read(fd_content, buf, buf_size)) > 0) {
            error_code = _split_contents_core(buf, bytes, ref_id, content_id, mongo_db_id, n_line, n_block, line, &bytes_in_line, &content_block);
            if (error_code) break;

            len -= bytes;
        }
    }

    if (!error_code) {
        error_code = _split_contents_deal_with_last_line_block(bytes_in_line, line, ref_id, content_id, mongo_db_id, &content_block, n_line, n_block);
    }

    destroy_content_block(&content_block);

    return error_code;
}

Err
delete_content(UUID content_id, enum MongoDBId mongo_db_id)
{
    Err error_code = S_OK;
    bson_t *key = BCON_NEW("the_id", BCON_BINARY(content_id, UUIDLEN));

    error_code = db_remove(mongo_db_id, key);

    bson_destroy(key);

    return error_code;
}

Err
init_content_block(ContentBlock *content_block, UUID ref_id, UUID content_id, int block_id)
{
    if (content_block->len_block) {
        bzero(content_block->buf_block, content_block->len_block);
    }

    content_block->len_block = 0;
    content_block->n_line = 0;
    memcpy(content_block->the_id, content_id, sizeof(UUID));
    memcpy(content_block->ref_id, ref_id, sizeof(UUID));

    content_block->block_id = block_id;

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
init_content_block_with_buf_block(ContentBlock *content_block, UUID ref_id, UUID content_id, int block_id)
{
    Err error_code = init_content_block_buf_block(content_block);

    if(error_code) return error_code;

    error_code = init_content_block(content_block, ref_id, content_id, block_id);
    if (error_code) return error_code;

    return S_OK;
}

Err
init_content_block_buf_block(ContentBlock *content_block)
{
    if(content_block->buf_block != NULL) return S_OK;

    content_block->buf_block = malloc(MAX_BUF_SIZE);
    if(content_block->buf_block == NULL) return S_ERR;

    content_block->max_buf_len = MAX_BUF_SIZE;
    bzero(content_block->buf_block, MAX_BUF_SIZE);    
    content_block->len_block = 0;

    return S_OK;
}

Err
destroy_content_block(ContentBlock *content_block)
{
    if (content_block->buf_block == NULL) return S_OK;

    free(content_block->buf_block);
    content_block->buf_block = NULL;
    content_block->max_buf_len = 0;

    return S_OK;
}

Err
associate_content_block(ContentBlock *content_block, char *buf_block, int max_buf_len)
{
    if(content_block->buf_block != NULL) return S_ERR;

    content_block->buf_block = buf_block;
    content_block->max_buf_len = max_buf_len;

    return S_OK;
}

Err
dissociate_content_block(ContentBlock *content_block)
{
    if (content_block->buf_block == NULL) return S_OK;

    content_block->buf_block = NULL;
    content_block->max_buf_len = 0;

    return S_OK;
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
        error_code = _split_contents_core_core(line, *bytes_in_line, ref_id, content_id, mongo_db_id, content_block, n_line, n_block);
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
    // check for max-buf in block-buf
    if (content_block->n_line  > MAX_BUF_LINES ||
        content_block->len_block + bytes_in_line > MAX_BUF_BLOCK) {

        error_code = _save_content_block(content_block, mongo_db_id);
        if (error_code) return error_code;

        error_code = init_content_block(content_block, ref_id, content_id, *n_block);
        (*n_block)++;
        if (error_code) return error_code;
    }

    memcpy(content_block->buf_block + content_block->len_block, line, bytes_in_line);
    content_block->len_block += bytes_in_line;

    // 1 more line
    if (line[bytes_in_line - 2] == '\r' && line[bytes_in_line - 1] == '\n') {
        (*n_line)++;
        content_block->n_line++;
    }

    return S_OK;
}

Err
save_content_block(ContentBlock *content_block, enum MongoDBId mongo_db_id)
{
    Err error_code = S_OK;
    if(content_block->buf_block == NULL) return S_ERR;

    bson_t *content_block_bson = NULL;
    bson_t *content_block_id_bson = NULL;

    error_code = _serialize_content_block_bson(content_block, &content_block_bson);

    if (!error_code) {
        error_code = _serialize_content_uuid_bson(content_block->the_id, content_block->block_id, &content_block_id_bson);
    }

    if (!error_code) {
        error_code = db_update_one(mongo_db_id, content_block_id_bson, content_block_bson, true);
    }

    bson_safe_destroy(&content_block_bson);
    bson_safe_destroy(&content_block_id_bson);

    return error_code;
}

Err
read_content_block(UUID content_id, int block_id, enum MongoDBId mongo_db_id, ContentBlock *content_block)
{
    Err error_code = S_OK;

    if(content_block->buf_block == NULL) return S_ERR;

    bson_t *key = BCON_NEW(
        "the_id", BCON_BINARY(content_id, UUIDLEN),
        "block_id", BCON_INT32(block_id)
        );

    bson_t *db_result = NULL;

    error_code = db_find_one(mongo_db_id, key, NULL, &db_result);

    if(!error_code) {
        error_code = _deserialize_content_block_bson(db_result, content_block);
    }

    bson_safe_destroy(&db_result);
    bson_safe_destroy(&key);

    return error_code;
}

Err
_split_contents_deal_with_last_line_block(int bytes_in_line, char *line, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, ContentBlock *content_block, int *n_line, int *n_block)
{
    Err error_code = S_OK;

    // last line
    if (bytes_in_line) {
        // Main-op
        error_code = _split_contents_core_core(line, bytes_in_line, ref_id, content_id, mongo_db_id, content_block, n_line, n_block);
        if (error_code) return error_code;
    }

    // last block
    if (content_block->len_block) {
        error_code = _save_content_block(content_block, mongo_db_id);
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
_serialize_content_block_bson(ContentBlock *content_block, bson_t **content_block_bson)
{
    *content_block_bson = BCON_NEW(
                              "the_id", BCON_BINARY(content_block->the_id, UUIDLEN),
                              "ref_id", BCON_BINARY(content_block->ref_id, UUIDLEN),
                              "block_id", BCON_INT32(content_block->block_id),
                              "len_block", BCON_INT32(content_block->len_block),
                              "n_line", BCON_INT32(content_block->n_line),
                              "buf_block", BCON_BINARY((unsigned char *)content_block->buf_block, content_block->len_block)
                          );

    return S_OK;
}

/**
 * @brief Deserialize bson to content-block
 * @details Deserialize bson to content-block (receive only len_block, not dealing with '\0' in the end)
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

    if(content_block->buf_block == NULL) return S_ERR;

    int len;
    error_code = bson_get_value_bin(content_block_bson, "the_id", UUIDLEN, (char *)content_block->the_id, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(content_block_bson, "ref_id", UUIDLEN, (char *)content_block->ref_id, &len);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(content_block_bson, "block_id", &content_block->block_id);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(content_block_bson, "len_block", &content_block->len_block);
    if (error_code) return error_code;

    error_code = bson_get_value_int32(content_block_bson, "n_line", &content_block->n_line);
    if (error_code) return error_code;

    error_code = bson_get_value_bin(content_block_bson, "buf_block", content_block->len_block, content_block->buf_block, &len);
    if (error_code) return error_code;

    return S_OK;
}
