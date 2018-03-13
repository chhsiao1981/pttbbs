#include "pttdb.h"
#include "pttdb_internal.h"

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
_split_contents_core(char *buf, int bytes, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, int *n_line, int *n_block, int *bytes_in_line, Content *content)
{
    Err error_code;
    char line[MAX_BUF_SIZE];

    int bytes_in_new_line = 0;
    for (int offset_buf = 0; offset_buf < bytes; offset_buf += bytes_in_new_line) {
        error_code = get_line_from_buf(buf, offset_buf, bytes, line, *bytes_in_line, &bytes_in_new_line);
        *bytes_in_line += bytes_in_new_line;
        if (error_code) {
            // unable to get more lines from buf
            break;
        }

        // Main-op
        error_code = _split_contents_core_core(line, *bytes_in_line, ref_id, content_id, content, n_line, n_block);
        if (error_code) {
            return error_code;
        }

        // reset line
        bzero(line, sizeof(char) * *bytes_in_line);
        *bytes_in_line = 0;
    }

    return S_OK;
}

Err
_split_contents_core_core(char *line, int bytes_in_line, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, Content *content, int *n_line, int *n_block)
{
    Err error_code;

    // check for max-lines in block-buf
    if(content->n_line  > MAX_BUF_LINES) {
        error_code = _save_content_block(content, mongo_db_id);
        if(error_code) {
            return error_code;
        }

        error_code = _init_content(content, ref_id, content_id, n_block);
        if(error_code) {
            return error_code;
        }
    }
    // XXX should never happen.
    else if (content->len_block + bytes_in_line > MAX_BUF_BLOCK) {
        error_code = =_save_content_block(content, mongo_db_id);
        if(error_code) {
            return error_code;
        }

        error_code = _init_content(content, ref_id, content_id, n_block);
        if(error_code) {
            return error_code;
        }
    }

    memcpy(content->buf_block + content->len_block, line, bytes_in_line);
    content->len_block += bytes_in_line;

    // 1 more line
    if(line[bytes_in_line - 2] == '\r' && line[bytes_in_line - 1] == '\n') {
        (*n_line)++;
        content->n_line++;
    }

    return S_OK;
}

Err
_init_content(Content *content, UUID ref_id, UUID content_id, int *block_id)
{
    if(content->len_block) {
        bzero(content->buf_block, content->len_block);
    }

    content->len_block = 0;
    content->n_line = 0;
    memcpy(content->the_id, content_id, sizeof(UUID));
    memcpy(content->ref_id, content_id, sizeof(UUID));

    content->block_id = *block_id;
    (*block_id)++;

    return S_OK;
}

Err
_save_content_block(Content *content, enum MongoDBId mongo_db_id)
{
    Error error_code;
    bson_t content_bson;
    bson_t content_id_bson;

    bson_init(&content_bson);
    bson_init(&content_id_bson);

    error_code = _serialize_content_bson(content, &content_bson);
    if(error_code) {
        bson_destroy(&content_bson);
        bson_destroy(&content_id_bson);
        return error_code;
    }

    error_code = _serialize_content_id_bson(content->the_id, MONGO_THE_ID, content->block_id, &content_id_bson);
    if(error_code) {
        bson_destroy(&content_bson);
        bson_destroy(&content_id_bson);
        return error_code;        
    }

    error_code = db_update_one(mongo_db_id, &content_id_bson, &content_bson, true);
    bson_destroy(&content_bson);
    bson_destroy(&content_id_bson);

    if(error_code) {
        return error_code;
    }

    return S_OK;
}
