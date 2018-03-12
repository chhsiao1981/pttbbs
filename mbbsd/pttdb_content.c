#include "pttdb.h"
#include "pttdb_internal.h"

Err
_split_contents(int fd_content, int len, UUID ref_id, UUID content_id, MongoDBId mongo_db_id, int *n_line, int *block)
{
    Err error_code;
    char buf[MAX_BUF_SIZE];
    char line[MAX_BUF_SIZE] = {};

    int bytes = 0;
    int buf_size = 0;
    int bytes_in_line = 0;
    int bytes_in_new_line = 0;

    Content conten_block = {};

    // init
    *n_block = 0;
    error_code = _init_content(&content_block, ref_id, content_id, n_block);
    if(error_code) {
        return error_code;
    }

    // while-loop
    while((buf_size = len < MAX_BUF_SIZE ? len: MAX_BUF_SIZE) && (bytes = read(fd_content, buf, buf_size)) > 0) {
        for(int offset_buf = 0; offset_buf < bytes; offset_buf += bytes_in_new_line) {
            error_code = get_line_from_buf(buf, offset_buf, bytes, line, bytes_in_line, &bytes_in_new_line);
            bytes_in_line += bytes_in_new_line;
            if(error_code) {
                break;
            }

            // main-op
            error_code = _split_contents_core(line, bytes_in_line, ref_id, content_id, &content_block, n_line, n_block);
            if(error_code) {
                return error_code;
            }

            // reset line
            bzero(line, bytes_in_line);
            bytes_in_line = 0;
        }

        len -= bytes;
    }
    // last line
    if(bytes_in_line) {
        // main-op
        error_code = _split_contents_core(line, bytes_in_line, ref_id, content_id, &content_block, n_line, n_block);
        if(error_code) {
            return error_code;
        }
    }
    // last block
    if(conten_block.len_block) {
        error_code = _save_content_block(&conten_block, mongo_db_id);
        if(error_code) {
            return error_code;
        }
    }

    return S_OK;
}

/**
 * @brief Initialize content
 * @details Set ref_id, content_id, and block_id. Others as 0
 *
 * @param content_block content-block
 * @param ref_id ref_id
 * @param content_id content_id
 * @param block_id block_id
 * @return Err
 */
Err
_init_content(Content *content_block, UUID ref_id, UUID content_id, int *block_id)
{
    if (content_block->len_block) {
        bzero(content_block->buf_block, content_block->len_block);
    }
    content_block->len_block = 0;
    content_block->n_line = 0;
    strncpy(content_block->the_id, content_id, sizeof(UUID));
    strncpy(content_block->ref_id, ref_id, sizeof(UUID));
    content_block->block_id = *block_id;

    (*block_id)++;

    return S_OK;
}

/**
 * @brief Core of _split_contents.
 * @details Core of _split_content.
 *          1. 1 more line.
 *          2. check whether the buffer exceeds limits.
 *          3. setup content->buffer, content->len, content->line.
 *
 * @param line line
 * @param bytes_in_line bytes_in_line
 * @param main_content_block main_content_block
 * @param n_line n_line
 * @param n_block n_block
 * @return Err
 */
Err
_split_contents_core(char *line, int bytes_in_line, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, Content *content_block, int *n_line, int *n_block)
{
    Err error_code;

    // check for max-lines in block-buf.
    if (content_block->n_line > MAX_BUF_LINES) {
        error_code = _save_content_block(content_block, mongo_db_id);
        if (error_code) {
            return error_code;
        }

        error_code = _init_content(content_block, ref_id, content_id, n_block);
        if (error_code) {
            return error_code;
        }
    }
    // XXX should never happen.
    else if (content_block->len_block + bytes_in_line > MAX_BUF_BLOCK) {
        error_code = _save_content_block(content_block, mongo_db_id);
        if (error_code) {
            return error_code;
        }

        error_code = _init_content(content_block, ref_id, content_id, n_block);
        if (error_code) {
            return error_code;
        }
    }

    strncpy(content_block->buf_block + content_block->len_block, line, bytes_in_line);
    content_block->len_block += bytes_in_line;

    //1 more line
    if (line[bytes_in_line - 2] == '\r' && line[bytes_in_line - 1] == '\n') {
        (*n_line)++;
        content_block->n_line++;
    }


    return S_OK;
}

/**
 * @brief Save content-block
 * @details Save content-block
 *
 * @param content_block content-block
 * @return Err
 */
Err
_save_content_block(Content *content_block, enum MongoDBId mongo_db_id)
{
    Err error_code;
    bson_t content_block_bson;
    bson_t content_block_uuid_bson;

    bson_init(&content_block_bson);
    bson_init(&content_block_uuid_bson);

    error_code = _serialize_content_block_bson(content_block, &content_block_bson);
    if (error_code) {
        bson_destroy(&content_block_bson);
        bson_destroy(&content_block_uuid_bson);
        return error_code;
    }

    error_code = _serialize_content_uuid_bson(content_block->the_id, MONGO_THE_ID, content_block->block_id, &content_block_uuid_bson);
    if (error_code) {
        bson_destroy(&content_block_bson);
        bson_destroy(&content_block_uuid_bson);
        return error_code;
    }

    error_code = db_update_one(mongo_db_id, &content_block_uuid_bson, &content_block_bson, true);
    bson_destroy(&content_block_uuid_bson);
    bson_destroy(&content_block_bson);
    if (error_code) {
        return error_code;
    }

    return S_OK;
}
