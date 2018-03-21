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

    return error_code;
}

Err
destroy_file_info(FileInfo *file_info)
{
    if(!file_info) return S_OK;

    if(file_info->content_block_info) {
        free(file_info->content_block_info);
    }
    if(file_info->page_info) {
        free(file_info->page_info);
    }
    bzero(file_info, sizeof(FileInfo));

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
