#include "cmpttui/vedit3_buffer.h"
#include "cmpttui/vedit3_buffer_private.h"

bool
vedit3_buffer_is_end_ne(VEdit3Buffer *buffer) {
    for (; buffer; buffer = buffer->next) {
        if (!buffer->is_to_delete) return false;
    }

    return true;
}

VEdit3Buffer *
vedit3_buffer_next_ne(VEdit3Buffer *buffer) {
    if (!buffer) return NULL;

    VEdit3Buffer *p_next = buffer->next;
    for (; p_next && p_next->is_to_delete; p_next = p_next->next);

    return p_next;
}

VEdit3Buffer *
vedit3_buffer_pre_ne(VEdit3Buffer *buffer) {
    if(!buffer) return NULL;

    VEdit3Buffer *p_pre = buffer->pre;
    for(; p_pre && p_pre->is_to_delete; p_pre = p_pre->pre);

    return p_pre;
}

Err
safe_free_vedit3_buffer(VEdit3Buffer **buffer)
{
    VEdit3Buffer *p_buffer = *buffer;
    if(!p_buffer) return S_OK;

    if(p_buffer->buf) free(p_buffer->buf);
    free(p_buffer);
    *buffer = NULL;

    return S_OK;
}

Err
destroy_vedit3_buffer_info(VEdit3BufferInfo *buffer_info)
{
    Err error_code = pttui_thread_lock_wrlock(LOCK_VEDIT3_BUFFER_INFO);
    fprintf(stderr, "vedit3_buffer.destroy_vedit3_buffer_info: after wrlock: e: %d\n", error_code);
    if (error_code) return error_code;

    VEdit3Buffer *p_buffer = buffer_info->head;
    VEdit3Buffer *tmp = NULL;
    while (p_buffer != NULL) {
        tmp = p_buffer;
        p_buffer = p_buffer->next;
        safe_free_vedit3_buffer(&tmp);
    }

    bzero(buffer_info, sizeof(VEdit3BufferInfo));

    error_code = pttui_thread_lock_unlock(LOCK_VEDIT3_BUFFER_INFO);
    fprintf(stderr, "vedit3_buffer.destroy_vedit3_buffer_info: after unlock: e: %d\n", error_code);
    
    return error_code;
}

Err
vedit3_buffer_is_begin_of_file(VEdit3Buffer *buffer, FileInfo *file_info GCC_UNUSED, bool *is_begin)
{
    if (buffer->content_type == PTTDB_CONTENT_TYPE_MAIN &&
        buffer->block_offset == 0 &&
        buffer->line_offset == 0) {
        *is_begin = true;
    }
    else {
        *is_begin = false;
    }

    return S_OK;
}

Err
vedit3_buffer_is_eof(VEdit3Buffer *buffer, FileInfo *file_info, bool *is_eof)
{
    if (!file_info->n_comment &&
        buffer->block_offset == file_info->n_main_block - 1 &&
        buffer->line_offset == file_info->main_blocks[buffer->block_offset].n_line - 1) {
        *is_eof = true;
    }
    else if (file_info->n_comment &&
        buffer->comment_offset == file_info->n_comment - 1 &&
        buffer->content_type == PTTDB_CONTENT_TYPE_COMMENT &&
        file_info->comments[buffer->comment_offset].n_comment_reply_block == 0) {
        *is_eof = true;
    }
    else if (file_info->n_comment &&
        buffer->comment_offset == file_info->n_comment - 1 &&
        buffer->content_type == PTTDB_CONTENT_TYPE_COMMENT_REPLY &&
        file_info->comments[buffer->comment_offset].n_comment_reply_block &&
        buffer->block_offset == file_info->comments[buffer->comment_offset].n_comment_reply_block - 1 &&
        buffer->line_offset == file_info->comments[buffer->comment_offset].comment_reply_blocks[buffer->block_offset].n_line - 1) {
        *is_eof = true;
    }
    else {
        *is_eof = false;
    }

    return S_OK;
}

/**********
 * sync vedit3 buffer info
 **********/
Err
sync_vedit3_buffer_info(VEdit3BufferInfo *buffer_info, VEdit3Buffer *current_buffer, VEdit3State *state, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    Err error_code = S_OK;

    // no buffer in buffer_info
    if (!current_buffer) {
        return resync_all_vedit3_buffer_info(buffer_info, state, file_info, new_buffer);
    }

    // determine new buffer of the expected-state
    bool tmp_is_pre = false;
    error_code = _sync_vedit3_buffer_info_is_pre(state, current_buffer, &tmp_is_pre);
    if (error_code) return error_code;

    error_code = _sync_vedit3_buffer_info_get_buffer(state, current_buffer, tmp_is_pre, new_buffer);
    if (error_code) return error_code;

    // found buffer in the current buffer-info
    if(*new_buffer) return S_OK;

    // not found buffer in the current buffer-info => resync-all
    return resync_all_vedit3_buffer_info(buffer_info, state, file_info, new_buffer);
}

Err
_sync_vedit3_buffer_info_is_pre(VEdit3State *state, VEdit3Buffer *buffer, bool *is_pre)
{
    // content-type as main
    if (state->top_line_content_type == PTTDB_CONTENT_TYPE_MAIN && buffer->content_type != PTTDB_CONTENT_TYPE_MAIN) {
        *is_pre = true;
        return S_OK;
    }

    if (state->top_line_content_type != PTTDB_CONTENT_TYPE_MAIN && buffer->content_type == PTTDB_CONTENT_TYPE_MAIN) {
        *is_pre = false;
        return S_OK;
    }

    // both are as main
    if (state->top_line_content_type == PTTDB_CONTENT_TYPE_MAIN && buffer->content_type == PTTDB_CONTENT_TYPE_MAIN) {
        if (state->top_line_block_offset != buffer->block_offset) {
            *is_pre = state->top_line_block_offset < buffer->block_offset ? true : false;
        }
        else {
            *is_pre = state->top_line_line_offset < buffer->line_offset ? true : false;
        }
        return S_OK;
    }

    /***
     * both are not main
     */
    // comment-offset
    if (state->top_line_comment_offset < buffer->comment_offset) {
        *is_pre = true;
        return S_OK;
    }

    if (state->top_line_comment_offset > buffer->comment_offset) {
        *is_pre = false;
        return S_OK;
    }

    // same comment-offset, compare content-type
    if (state->top_line_content_type == PTTDB_CONTENT_TYPE_COMMENT && buffer->content_type != PTTDB_CONTENT_TYPE_COMMENT) {
        *is_pre = true;
        return S_OK;
    }

    if (state->top_line_content_type != PTTDB_CONTENT_TYPE_COMMENT && buffer->content_type == PTTDB_CONTENT_TYPE_COMMENT) {
        *is_pre = false;
        return S_OK;
    }

    // both are comment-reply
    if (state->top_line_content_type == PTTDB_CONTENT_TYPE_COMMENT_REPLY && buffer->content_type == PTTDB_CONTENT_TYPE_COMMENT_REPLY) {
        if (state->top_line_block_offset != buffer->block_offset) {
            *is_pre = state->top_line_block_offset < buffer->block_offset ? true : false;
        }
        else {
            *is_pre = state->top_line_line_offset < buffer->line_offset ? true : false;
        }
        return S_OK;
    }

    *is_pre = false;
    return S_OK;
}

Err
_sync_vedit3_buffer_info_get_buffer(VEdit3State *state, VEdit3Buffer *current_buffer, bool is_pre, VEdit3Buffer **new_buffer)
{
    VEdit3Buffer *p_buffer = current_buffer;

    while (p_buffer != NULL) {
        if (!memcmp(state->top_line_id, p_buffer->the_id, UUIDLEN) &&
                state->top_line_block_offset == p_buffer->block_offset &&
                state->top_line_line_offset == p_buffer->line_offset) {
            break;
        }

        p_buffer = is_pre ? p_buffer->pre : p_buffer->next;
    }

    *new_buffer = p_buffer;

    return S_OK;
}

/**
 * @brief [brief description]
 * @details 1. destroy vedit3_buffer_info.
 *          2. initialize one buffer.
 *          3. extend pre-buffer based on the new buffer.
 *          4. extend next-buffer based on the new buffer.
 *          5. setup buffer to buffer_info.
 * 
 * 
 * @param buffer_info [description]
 * @param state [description]
 * @param file_info [description]
 * @param new_buffer [description]
 */
Err
resync_all_vedit3_buffer_info(VEdit3BufferInfo *buffer_info, VEdit3State *state, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    Err error_code = S_OK;

    // lock for writing buffer-info
    error_code = pttui_thread_lock_wrlock(LOCK_VEDIT3_WR_BUFFER_INFO);
    if(error_code) return error_code;

    VEdit3Buffer *p_buffer = buffer_info->head;
    VEdit3Buffer *p_tail_buffer = buffer_info->tail;

    // 1. save all buffer to tmp_file

    error_code = _save_vedit3_buffer_info_to_tmp_file(buffer_info);

    // 2. destroy buffer_info
    if(!error_code) {
        error_code = destroy_vedit3_buffer_info(buffer_info);
    }

    // 3. init-one-buf
    VEdit3Buffer *tmp_buffer = NULL;
    VEdit3Buffer *tmp_head = NULL;
    VEdit3Buffer *tmp_tail = NULL;
    int n_buffer = 0;

    if(!error_code) {
        error_code = _vedit3_buffer_init_buffer_no_buf_from_file_info(state, file_info, &tmp_buffer);
    }

    if(!error_code) {
        n_buffer++;        
        *new_buffer = tmp_buffer;
    }

    // 4. extend vedit3 buffer
    if(!error_code) {
        error_code = _extend_vedit3_buffer(state, file_info, tmp_buffer, tmp_buffer, tmp_buffer, &tmp_head, &tmp_tail, &n_buffer);
    }

    // 5. set to buffer-info
    Err error_code_lock = S_ERR_NOT_INIT;
    if(!error_code) {
        error_code_lock = pttui_thread_lock_wrlock(LOCK_VEDIT3_BUFFER_INFO);
    }

    if(!error_code && !error_code_lock) {
        buffer_info->head = tmp_head;
        buffer_info->tail = tmp_tail;
        buffer_info->n_buffer = n_buffer;
        memcpy(buffer_info->main_id, state->main_id, UUIDLEN);
    }

    // unlock
    if(!error_code_lock) {
        error_code_lock = pttui_thread_lock_unlock(LOCK_VEDIT3_BUFFER_INFO);
        if (!error_code && error_code_lock) error_code = error_code_lock;
    }

    error_code_lock = pttui_thread_lock_unlock(LOCK_VEDIT3_WR_BUFFER_INFO);
    if(!error_code && error_code_lock) error_code = error_code_lock;

    return error_code;
}

/**********
 * init buffer
 **********/

/**
 * @brief [brief description]
 * @details XXX ASSUME no new-line in the file-info (used only in resync-all)
 * 
 * @param state [description]
 * @param file_info [description]
 * @param buffer [description]
 * @param o [description]
 * @param r [description]
 */
Err
_vedit3_buffer_init_buffer_no_buf_from_file_info(VEdit3State *state, FileInfo *file_info, VEdit3Buffer **buffer)
{
    *buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *p_buffer = *buffer;
    if(!p_buffer) return S_ERR_MALLOC;
    
    bzero(p_buffer, sizeof(VEdit3Buffer));

    memcpy(p_buffer->the_id, state->top_line_id, UUIDLEN);
    p_buffer->content_type = state->top_line_content_type;
    p_buffer->block_offset = state->top_line_block_offset;
    p_buffer->line_offset = state->top_line_line_offset;
    p_buffer->comment_offset = state->top_line_comment_offset;
    p_buffer->load_line_offset = p_buffer->line_offset;
    p_buffer->load_line_pre_offset = p_buffer->load_line_offset ? p_buffer->load_line_offset - 1 : INVALID_LINE_OFFSET_PRE_END;

    CommentInfo *p_comment_info = NULL;
    ContentBlockInfo *p_content_block_info = NULL;

    switch (p_buffer->content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        p_content_block_info = file_info->main_blocks + p_buffer->block_offset;
        p_buffer->storage_type = p_content_block_info->storage_type;
        p_buffer->load_line_next_offset = p_buffer->load_line_offset < p_content_block_info->n_line - 1 ? p_buffer->load_line_offset + 1 : INVALID_LINE_OFFSET_NEXT_END;
        break;
    case PTTDB_CONTENT_TYPE_COMMENT:
        p_buffer->storage_type = PTTDB_STORAGE_TYPE_MONGO;
        p_buffer->load_line_next_offset = INVALID_LINE_OFFSET_NEXT_END;
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        p_comment_info = file_info->comments + p_buffer->comment_offset;
        p_content_block_info = p_comment_info->comment_reply_blocks + p_buffer->block_offset;
        p_buffer->storage_type = p_content_block_info->storage_type;
        p_buffer->load_line_next_offset = p_buffer->load_line_offset < p_content_block_info->n_line - 1 ? p_buffer->load_line_offset + 1 : INVALID_LINE_OFFSET_NEXT_END;
        break;
    default:
        break;
    }

    return S_OK;
}

/**********
 * extend vedit3 buffer
 **********/


Err
extend_vedit3_buffer_info(VEdit3State *state, FileInfo *file_info,)

/**
 * @brief [brief description]
 * @details XXX ASSUME head_buffer, tail_buffer, current_buffer exists
 *          XXX require LOCK_VEDIT3_WR_BUFFER_INFO already locked
 *          1. count extra-pre-range
 *          2. if need extend pre: extend-pre
 *          3. if need extend next: extend-next
 * 
 * @param buffer_info [description]
 * @param current_buffer [description]
 */
Err
_extend_vedit3_buffer(FileInfo *file_info, VEdit3Buffer *head_buffer, VEdit3Buffer *tail_buffer, VEdit3Buffer *current_buffer, VEdit3Buffer **new_head_buffer, VEdit3Buffer **new_tail_buffer)
{
    // 1. determine extra-range required to retrieve.
    // 2. do extend buffer.
    Err error_code = S_OK;

    int n_extra_pre_range = 0;
    int n_extra_next_range = 0;

    error_code = _extend_vedit3_buffer_count_extra_pre_range(current_buffer, &n_extra_range);

    if(!error_code) {
        error_code = _extend_vedit3_buffer_count_extra_next_range(current_buffer, &n_extra_range);
    }

    // extend-pre
    if(!error_code && (!head_buffer->buf || n_extra_pre_range)) {
        error_code = _extend_vedit3_buffer_extend_pre_buffer(file_info, head_buffer, n_extra_range, new_head_buffer, ret_n_buffer);
    }

    // extend-next
    if (!error_code && (!tail_buffer->buf || n_extra_next_range)) {
        error_code = _extend_vedit3_buffer_extend_next_buffer(file_info, tail_buffer, n_extra_range, new_tail_buffer, ret_n_buffer);
    }

    return error_code;
}

Err
_extend_vedit3_buffer_count_extra_pre_range(VEdit3Buffer *buffer, int *n_extra_range)
{
    VEdit3Buffer *p_buffer = NULL;
    int i = 0;
    for (i = 0, p_buffer = buffer; i < SOFT_N_VEDIT3_BUFFER && p_buffer && p_buffer != head; i++, p_buffer = p_buffer->pre);

    *n_extra_range = i == SOFT_N_VEDIT3_BUFFER ? 0 : (HARD_N_VEDIT3_BUFFER - i);

    return S_OK;
}

Err
_extend_vedit3_buffer_count_extra_next_range(VEdit3Buffer *buffer, int *n_extra_range)
{
    VEdit3Buffer *p_buffer = NULL;

    int i = 0;
    for (i = 0, p_buffer = buffer; i < SOFT_N_VEDIT3_BUFFER && p_buffer; i++, p_buffer = p_buffer->next);

    *n_extra_range = i == SOFT_N_VEDIT3_BUFFER ? 0 : (HARD_N_VEDIT3_BUFFER - i);

    //fprintf(stderr, "vedit3_buffer._sync_vedit3_buffer_info_count-extra_next_range: i: %d n_extra_range: %d\n", i, *n_extra_range);

    return S_OK;
}

/**********
 * extend pre buffer
 **********/
Err
_extend_vedit3_buffer_extend_pre_buffer(FileInfo *file_info, VEdit3Buffer *head_buffer, int n_buffer, VEdit3Buffer **new_head_buffer, int *ret_n_buffer)
{    
    Err error_code = S_OK;
    VEdit3Buffer *start_buffer = head_buffer;

    // 1. check begin-of-file
    bool is_begin = false;
    if(start_buffer && start_buffer->buf) {
        error_code = vedit3_buffer_is_begin_of_file(start_buffer, file_info, &is_begin);
        if(error_code) return error_code;
        if(is_begin) return S_OK;
    }

    // 2. extend-pre-buffer-no-buf
    VEdit3ResourceInfo resource_info = {};
    VEdit3ResourceDict resource_dict = {};
    error_code = _extend_vedit3_buffer_extend_pre_buffer_no_buf(start_buffer, file_info, n_buffer, new_head_buffer, ret_n_buffer);

    if (error_code) return error_code;

    if (start_buffer->buf) start_buffer = start_buffer->pre;

    error_code = _vedit3_buffer_info_to_resource_info(start_buffer, &resource_info);

    if (!error_code) {
        error_code = vedit3_resource_info_to_resource_dict(&resource_info, &resource_dict);
    }

    if (!error_code) {
        error_code = _vedit3_buffer_info_set_buf_from_resource_dict(start_buffer, &resource_dict);
    }

    // free
    destroy_vedit3_resource_info(&resource_info);
    safe_destroy_vedit3_resource_dict(&resource_dict);

    return error_code;
}

Err
_extend_vedit3_buffer_extend_pre_buffer_no_buf(VEdit3Buffer *current_buffer, FileInfo *file_info, int n_buffer, VEdit3Buffer **new_head_buffer, int *ret_n_buffer)
{
    Err error_code = S_OK;
    VEdit3Buffer *new_buffer = NULL;
    int i = 0;
    for (i = 0; i < n_buffer; i++) {
        error_code = _extend_vedit3_buffer_extend_pre_buffer_no_buf_core(current_buffer, file_info, &new_buffer);
        if (error_code) break;

        if (!new_buffer) break;

        new_buffer->next = current_buffer;
        current_buffer->pre = new_buffer;
        
        current_buffer = new_buffer;
        new_buffer = NULL;
    }
    *new_head_buffer = current_buffer;
    (*ret_n_buffer) += i;

    return error_code;
}

Err
_extend_vedit3_buffer_extend_pre_buffer_no_buf_core(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    Err error_code = S_OK;
    switch (current_buffer->content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        error_code = _extend_vedit3_buffer_extend_pre_buffer_no_buf_main(current_buffer, file_info, new_buffer);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT:
        error_code = _extend_vedit3_buffer_extend_pre_buffer_no_buf_comment(current_buffer, file_info, new_buffer);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        error_code = _extend_vedit3_buffer_extend_pre_buffer_no_buf_comment_reply(current_buffer, file_info, new_buffer);
        break;
    default:
        error_code = S_ERR;
        break;
    }

    return error_code;
}

Err
_extend_vedit3_buffer_extend_pre_buffer_no_buf_main(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    // begin-of-file
    bool is_begin = false;
    Err error_code = vedit3_buffer_is_begin_of_file(current_buffer, file_info, &is_begin);
    if(error_code) return error_code;    
    if(is_begin) return S_OK;

    // same-block, but no valid pre-offset
    if(current_buffer->line_offset != 0 && current_buffer->load_line_pre_offset < 0) return S_ERR_EXTEND;

    // malloc new buffer
    *new_buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *tmp = *new_buffer;
    bzero(tmp, sizeof(VEdit3Buffer));

    memcpy(tmp->the_id, current_buffer->the_id, UUIDLEN);
    tmp->content_type = current_buffer->content_type;
    tmp->comment_offset = 0;

    if (current_buffer->line_offset != 0) { // same-block
        tmp->block_offset = current_buffer->block_offset;
        tmp->line_offset = current_buffer->line_offset - 1;
        tmp->storage_type = current_buffer->storage_type;

        tmp->load_line_offset = current_buffer->load_line_pre_offset;
        tmp->load_line_next_offset = current_buffer->load_line_offset;
        tmp->load_line_pre_offset = tmp->load_line_offset ? tmp->load_line_offset - 1 : INVALID_LINE_OFFSET_PRE_END;
    }
    else { // new block, referring to file-info
        tmp->block_offset = current_buffer->block_offset - 1;
        tmp->line_offset = file_info->main_blocks[tmp->block_offset].n_line - 1;
        tmp->storage_type = file_info->main_blocks[tmp->block_offset].storage_type;

        tmp->load_line_offset = tmp->line_offset;
        tmp->load_line_pre_offset = tmp->load_line_offset ? tmp->load_line_offset - 1 : INVALID_LINE_OFFSET_PRE_END;
        tmp->load_line_next_offset = INVALID_LINE_OFFSET_NEXT_END;
    }

    return S_OK;
}

Err
_extend_vedit3_buffer_info_extend_pre_buffer_no_buf_comment(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    *new_buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *tmp = *new_buffer;
    bzero(tmp, sizeof(VEdit3Buffer));

    CommentInfo *p_comment = NULL;
    ContentBlockInfo *p_content_block = NULL;
    if (current_buffer->comment_offset == 0) { // main-block. referring to file-info
        tmp->content_type = PTTDB_CONTENT_TYPE_MAIN;
        memcpy(tmp->the_id, file_info->main_content_id, UUIDLEN);
        tmp->block_offset = file_info->n_main_block - 1;
        p_content_block = file_info->main_blocks + tmp->block_offset;
        tmp->line_offset = p_content_block->n_line - 1;
        tmp->comment_offset = 0;

        tmp->load_line_offset = tmp->line_offset;
        tmp->load_line_pre_offset = tmp->load_line_offset ? tmp->load_line_offset - 1 : INVALID_LINE_OFFSET_PRE_END;
        tmp->load_line_next_offset = INVALID_LINE_OFFSET_NEXT_END;

        tmp->storage_type = p_content_block->storage_type;        
    }
    else {
        tmp->comment_offset = current_buffer->comment_offset - 1;
        p_comment = file_info->comments + tmp->comment_offset;
        if (p_comment->n_comment_reply_total_line) { // comment-reply-block. referring to file-info
            tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
            memcpy(tmp->the_id, p_comment->comment_reply_id, UUIDLEN);
            tmp->block_offset = p_comment->n_comment_reply_block - 1;

            p_content_block = p_comment->comment_reply_blocks + tmp->block_offset;
            tmp->line_offset = p_content_block->n_line - 1;

            tmp->load_line_offset = tmp->line_offset;
            tmp->load_line_pre_offset = tmp->load_line_offset ? tmp->load_line_offset - 1 : INVALID_LINE_OFFSET_PRE_END;
            tmp->load_line_next_offset = INVALID_LINE_OFFSET_NEXT_END;

            tmp->storage_type = p_content_block->storage_type;
        }
        else {
            tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT; // comment
            memcpy(tmp->the_id, p_comment->comment_id, UUIDLEN);
            tmp->block_offset = 0;
            tmp->line_offset = 0;

            tmp->load_line_offset = tmp->line_offset;
            tmp->load_line_pre_offset = INVALID_LINE_OFFSET_PRE_END;
            tmp->load_line_next_offset = INVALID_LINE_OFFSET_NEXT_END;

            tmp->storage_type = PTTDB_STORAGE_TYPE_MONGO;
        }
    }

    return S_OK;
}

Err
_extend_vedit3_buffer_info_extend_pre_buffer_no_buf_comment_reply(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    // same-block, but no valid pre-offset
    if(current_buffer->line_offset != 0 && current_buffer->load_line_pre_offset < 0) return S_ERR_EXTEND;

    *new_buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *tmp = *new_buffer;
    bzero(tmp, sizeof(VEdit3Buffer));

    tmp->comment_offset = current_buffer->comment_offset;

    CommentInfo *p_comment = file_info->comments + tmp->comment_offset;
    ContentBlockInfo *p_comment_reply_block = NULL;
    if (current_buffer->block_offset == 0 && current_buffer->line_offset == 0) { // comment
        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT;
        memcpy(tmp->the_id, p_comment->comment_id, UUIDLEN);
        tmp->block_offset = 0;
        tmp->line_offset = 0;

        tmp->load_line_offset = tmp->line_offset;
        tmp->load_line_pre_offset = INVALID_LINE_OFFSET_PRE_END;
        tmp->load_line_next_offset = INVALID_LINE_OFFSET_NEXT_END;

        tmp->storage_type = PTTDB_STORAGE_TYPE_MONGO;
    }
    else {
        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
        memcpy(tmp->the_id, current_buffer->the_id, UUIDLEN);

        if (current_buffer->line_offset != 0) { // same-block
            tmp->block_offset = current_buffer->block_offset;
            tmp->line_offset = current_buffer->line_offset - 1;

            tmp->load_line_offset = current_buffer->load_line_pre_offset;
            tmp->load_line_pre_offset = tmp->load_line_offset ? tmp->load_line_offset - 1 : INVALID_LINE_OFFSET_PRE_END;
            tmp->load_line_next_offset = current_buffer->load_line_offset;

            tmp->storage_type = current_buffer->storage_type;
        }
        else { // different block. referring to file-info
            tmp->block_offset = current_buffer->block_offset - 1;
            p_comment_reply_block = p_comment->comment_reply_blocks + tmp->block_offset;
            tmp->line_offset = p_comment_reply_block->n_line - 1;

            tmp->load_line_offset = tmp->line_offset;
            tmp->load_line_pre_offset = tmp->load_line_offset ? tmp->load_line_offset - 1 : INVALID_LINE_OFFSET_PRE_END;
            tmp->load_line_next_offset = INVALID_LINE_OFFSET_NEXT_END;

            tmp->storage_type = p_comment_reply_block->storage_type;
        }
    }

    return S_OK;
}

/**********
 * extend next buffer
 **********/
Err
_extend_vedit3_buffer_extend_next_buffer(FileInfo *file_info, VEdit3Buffer *tail_buffer, int n_buffer, VEdit3Buffer *new_tail_buffer, int *ret_n_buffer)
{
    Err error_code = S_OK;
    VEdit3Buffer *start_buffer = *tail_buffer;

    // 1. check eof
    bool is_eof = false;
    if (start_buffer && start_buffer->buf) {
        error_code = vedit3_buffer_is_eof(start_buffer, file_info, &is_eof);
        if (error_code) return error_code;
        if (is_eof) return S_OK;
    }

    // 2. extend-next-buffer-no-buf
    VEdit3ResourceInfo resource_info = {};
    VEdit3ResourceDict resource_dict = {};
    error_code = _extend_vedit3_buffer_extend_next_buffer_no_buf(start_buffer, file_info, n_buffer, new_tail_buffer, ret_n_buffer);

    if (error_code) return error_code;

    if (start_buffer->buf) start_buffer = start_buffer->next;

    error_code = _vedit3_buffer_info_to_resource_info(start_buffer, &resource_info);

    if (!error_code) {
        error_code = vedit3_resource_info_to_resource_dict(&resource_info, &resource_dict);
    }

    if (!error_code) {
        error_code = _vedit3_buffer_info_set_buf_from_resource_dict(start_buffer, &resource_dict);
    }

    // free
    destroy_vedit3_resource_info(&resource_info);
    safe_destroy_vedit3_resource_dict(&resource_dict);

    return error_code;
}

Err
_extend_vedit3_buffer_extend_next_buffer_no_buf(VEdit3Buffer *current_buffer, FileInfo *file_info, int n_buffer, VEdit3Buffer ***new_tail_buffer, int *ret_n_buffer)
{
    Err error_code = S_OK;
    VEdit3Buffer *new_buffer = NULL;
    int i = 0;
    for (i = 0; i < n_buffer; i++) {
        error_code = _extend_vedit3_buffer_extend_next_buffer_no_buf_core(current_buffer, file_info, &new_buffer);
        if (error_code) break;

        if (!new_buffer) break;

        new_buffer->pre = current_buffer;
        current_buffer->next = new_buffer;

        current_buffer = new_buffer;
        new_buffer = NULL;
    }
    *new_tail_buffer = current_buffer;
    (*ret_n_buffer) += i;

    return error_code;
}

Err
_extend_vedit3_buffer_info_extend_next_buffer_no_buf_core(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    Err error_code = S_OK;
    switch (current_buffer->content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        error_code = _extend_vedit3_buffer_extend_next_buffer_no_buf_main(current_buffer, file_info, new_buffer);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT:
        error_code = _extend_vedit3_buffer_extend_next_buffer_no_buf_comment(current_buffer, file_info, new_buffer);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        error_code = _extend_vedit3_buffer_extend_next_buffer_no_buf_comment_reply(current_buffer, file_info, new_buffer);
        break;
    default:
        error_code = S_ERR;
        break;
    }

    return error_code;
}

Err
_extend_vedit3_buffer_extend_next_buffer_no_buf_main(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    // eof
    bool is_eof = false;
    Err error_code = vedit3_buffer_is_eof(current_buffer, file_info, &is_eof);
    if(error_code) return error_code;    
    if(is_eof) return S_OK;

    // same-block, but no valid next-offset
    ContentBlockInfo *p_content_block = file_info->main_blocks + current_buffer->block_offset;    
    if(current_buffer->line_offset != p_content_block->n_line - 1 && current_buffer->load_line_next_offset < 0) return S_ERR_EXTEND;

    // malloc new buffer
    *new_buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *tmp = *new_buffer;
    bzero(tmp, sizeof(VEdit3Buffer));

    // last line of main-block. new-buffer as comment
    if (current_buffer->block_offset == file_info->n_main_block - 1 &&
            current_buffer->line_offset == p_content_block->n_line - 1) {

        memcpy(tmp->the_id, file_info->comments[0].comment_id, UUIDLEN);
        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT;
        tmp->comment_offset = 0;
        tmp->block_offset = 0;
        tmp->line_offset = 0;

        tmp->load_line_offset = 0;
        tmp->load_line_pre_offset = INVALID_LINE_OFFSET_PRE_END;
        tmp->load_line_next_offset = INVALID_LINE_OFFSET_NEXT_END;

        tmp->storage_type = PTTDB_STORAGE_TYPE_MONGO;
        return S_OK;
    }

    // still in main-block
    memcpy(tmp->the_id, current_buffer->the_id, UUIDLEN);
    tmp->content_type = current_buffer->content_type;
    tmp->comment_offset = 0;

    if (current_buffer->line_offset != p_content_block->n_line - 1) {
        // not the last-line
        tmp->block_offset = current_buffer->block_offset;
        tmp->line_offset = current_buffer->line_offset + 1;

        tmp->load_line_offset = current_buffer->load_line_next_offset;
        tmp->load_line_pre_offset = current_buffer->load_line_offset;
        tmp->load_line_next_offset = tmp->load_line_offset < p_content_block->n_line_in_db - 1 ? tmp->load_line_offset + 1 : INVALID_LINE_OFFSET_NEXT_END;

        tmp->storage_type = p_content_block->storage_type;
    }
    else {
        // last-line, but not the last block, referring from file-info
        tmp->block_offset = current_buffer->block_offset + 1;
        tmp->line_offset = 0;

        p_content_block++;

        tmp->load_line_offset = tmp->line_offset;
        tmp->load_line_pre_offset = INVALID_LINE_OFFSET_PRE_END;
        tmp->load_line_next_offset = tmp->load_line_offset < p_content_block->n_line_in_db - 1 ? tmp->load_line_offset + 1 : INVALID_LINE_OFFSET_NEXT_END;

        tmp->storage_type = p_content_block->storage_type;
    }

    return S_OK;
}

Err
_extend_vedit3_buffer_extend_next_buffer_no_buf_comment(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    // eof
    bool is_eof = false;
    Err error_code = vedit3_buffer_is_eof(current_buffer, file_info, &is_eof);
    if(error_code) return error_code;    
    if(is_eof) return S_OK;

    int current_buffer_comment_offset = current_buffer->comment_offset;
    CommentInfo *p_comment = file_info->comments + current_buffer_comment_offset;
    ContentBlockInfo *p_comment_reply_block = NULL;

    *new_buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *tmp = *new_buffer;
    bzero(tmp, sizeof(VEdit3Buffer));

    if (p_comment->n_comment_reply_block) {
        // with comment-reply // referring from file-info
        p_comment_reply_block = p_comment->comment_reply_blocks;

        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
        memcpy(tmp->the_id, p_comment->comment_reply_id, UUIDLEN);
        tmp->comment_offset = current_buffer_comment_offset;
        tmp->block_offset = 0;
        tmp->line_offset = 0;
        tmp->storage_type = p_comment_reply_block->storage_type;

        tmp->load_line_offset = tmp->line_offset;
        tmp->load_line_pre_offset = INVALID_LINE_OFFSET_PRE_END;
        tmp->load_line_next_offset = tmp->load_line_offset < p_comment_reply_block->n_line_in_db - 1 ? tmp->load_line_offset + 1 : INVALID_LINE_OFFSET_NEXT_END;
    }
    else {
        // next comment
        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT;

        p_comment++;

        memcpy(tmp->the_id, p_comment->comment_id, UUIDLEN);
        tmp->comment_offset = current_buffer_comment_offset + 1;
        tmp->block_offset = 0;
        tmp->line_offset = 0;
        tmp->storage_type = PTTDB_STORAGE_TYPE_MONGO;

        tmp->load_line_offset = 0;
        tmp->load_line_pre_offset = INVALID_LINE_OFFSET_PRE_END;
        tmp->load_line_next_offset = INVALID_LINE_OFFSET_NEXT_END;
    }

    return S_OK;
}

Err
_extend_vedit3_buffer_extend_next_buffer_no_buf_comment_reply(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    // eof
    bool is_eof = false;
    Err error_code = vedit3_buffer_is_eof(current_buffer, file_info, &is_eof);
    if(error_code) return error_code;    
    if(is_eof) return S_OK;

    CommentInfo *p_comment = file_info->comments + current_buffer->comment_offset;
    ContentBlockInfo *p_comment_reply_block = p_comment->comment_reply_blocks + current_buffer->block_offset;

    // same-block, but no valid load_line_next_offset
    if(current_buffer->line_offset != p_comment_reply_block->n_line - 1 &&
        current_buffer->load_line_next_offset < 0) return S_ERR_EXTEND;

    *new_buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *tmp = *new_buffer;
    bzero(tmp, sizeof(VEdit3Buffer));

    // same-block
    if (current_buffer->line_offset != p_comment_reply_block->n_line - 1) {
        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;        
        memcpy(tmp->the_id, p_comment->comment_reply_id, UUIDLEN);
        tmp->comment_offset = current_buffer_comment_offset;
        tmp->block_offset = current_buffer_block_offset;
        tmp->line_offset = current_buffer_line_offset + 1;

        tmp->load_line_offset = current_buffer->load_line_next_offset;
        tmp->load_line_pre_offset = current_buffer->load_line_offset;
        tmp->load_line_next_offset = tmp->load_line_offset < p_comment_reply_block->n_line_in_db - 1 ? tmp->load_line_offset + 1 : INVALID_LINE_OFFSET_NEXT_END;

        tmp->storage_type = p_comment_reply_block->storage_type;
    }
    else if (current_buffer->block_offset != p_comment->n_comment_reply_block - 1) {
        // different block, within same comment, referring file-info
        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
        memcpy(tmp->the_id, p_comment->comment_reply_id, UUIDLEN);
        tmp->comment_offset = current_buffer_comment_offset;
        tmp->block_offset = current_buffer_block_offset + 1;
        tmp->line_offset = 0;

        p_comment_reply_block++;

        tmp->load_line_offset = tmp->line_offset;
        tmp->load_line_pre_offset = INVALID_LINE_OFFSET_PRE_END;
        tmp->load_line_next_offset = tmp->load_line_offset < p_comment_reply_block->n_line_in_db - 1 ? tmp->load_line_offset + 1 : INVALID_LINE_OFFSET_NEXT_END;

        tmp->storage_type = p_comment_reply_block->storage_type;
    }
    else {
        // different comment
        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT;
        p_comment++;
        memcpy(tmp->the_id, p_comment->comment_id, UUIDLEN);
        tmp->comment_offset = current_buffer_comment_offset + 1;
        tmp->block_offset = 0;
        tmp->line_offset = 0;

        tmp->load_line_offset = 0;
        tmp->load_line_pre_offset = INVALID_LINE_OFFSET_PRE_END;
        tmp->load_line_next_offset = INVALID_LINE_OFFSET_NEXT_END;

        tmp->storage_type = PTTDB_STORAGE_TYPE_MONGO;
    }

    return S_OK;
}

/**********
 * buffer-info to resource-info
 **********/
Err
_vedit3_buffer_info_to_resource_info(VEdit3Buffer *head, VEdit3ResourceInfo *resource_info)
{
    Err error_code = S_OK;

    VEdit3Buffer *pre_buffer = NULL;
    for (VEdit3Buffer *current_buffer = head; current_buffer; current_buffer = current_buffer->next) {
        if (pre_buffer &&
            current_buffer->content_type != PTTDB_CONTENT_TYPE_COMMENT &&
            pre_buffer->content_type == current_buffer->content_type &&
            pre_buffer->block_offset == current_buffer->block_offset) continue;

        error_code = vedit3_resource_info_push_queue(current_buffer, resource_info, current_buffer->content_type, current_buffer->storage_type);
        if (error_code) break;

        pre_buffer = current_buffer;
    }

    return error_code;
}

/**********
 * resource-dict to buffer-info
 **********/
Err
_vedit3_buffer_info_set_buf_from_resource_dict(VEdit3Buffer *head, VEdit3ResourceDict *resource_dict)
{
    Err error_code = S_OK;

    VEdit3Buffer *p_buffer = head;
    UUID current_the_id = {};
    int current_block_id = 0;
    int len = 0;
    char *buf = NULL;
    char *p_buf = NULL;
    char *p_next_buf = NULL;
    char *p_buf_no_nl = NULL;
    int buf_offset = 0;
    int buf_next_offset = 0;
    int p_buffer_len = 0;
    int p_buffer_len_no_nl = 0;
    int i = 0;

    // XXX log
    //log_vedit3_resource_dict(resource_dict, "vedit3_buffer._vedit3_buffer_info_set_buf_from_resource_dict");

    // pre-head
    error_code = vedit3_resource_dict_get_data(resource_dict, p_buffer->the_id, p_buffer->block_offset, &len, &buf);
    if(error_code) return error_code;

    memcpy(current_the_id, p_buffer->the_id, UUIDLEN);
    current_block_id = p_buffer->block_offset;
    p_buf = buf;
    buf_offset = 0;

    for(int i = 0; i < p_buffer->load_line_offset; i++) {
        if(buf_offset == len) return S_ERR; // XXX should not be here

        error_code = _vedit3_buffer_info_set_buf_from_resource_dict_get_next_buf(p_buf, buf_offset, len, &p_next_buf, &buf_next_offset);
        if(error_code) return S_ERR;

        p_buf = p_next_buf;
        buf_offset = buf_next_offset;
    }

    // start
    for(; p_buffer && !p_buffer->buf; p_buffer = p_buffer->next, i++) {
        if(memcmp(p_buffer->the_id, current_the_id, UUIDLEN) || current_block_id != p_buffer->block_offset) {
            error_code = vedit3_resource_dict_get_data(resource_dict, p_buffer->the_id, p_buffer->block_offset, &len, &buf);
            fprintf(stderr, "vedit3_buffer._vedit3_buffer_info_set_buf_from_resource_dict: after get data: i: %d p_buffer: content-type: %d block_offset: %d comment_offset: %d len: %d e: %d\n", i, p_buffer->content_type, p_buffer->block_offset, p_buffer->comment_offset, len, error_code);

            if(error_code) break;

            memcpy(current_the_id, p_buffer->the_id, UUIDLEN);
            current_block_id = p_buffer->block_offset;
            p_buf = buf;
            buf_offset = 0;
        }
        if(buf_offset == len) { // XXX should not be here
            error_code = S_ERR;
            break;
        }        
        error_code = _vedit3_buffer_info_set_buf_from_resource_dict_get_next_buf(p_buf, buf_offset, len, &p_next_buf, &buf_next_offset);
        if(error_code) break;

        fprintf(stderr, "vedit3_buffer._vedit3_buffer_info_set_buf_from_resource_dict: i: %d content_type: %d buf_offset: %d buf_next_offset: %d len: %d p_next: %lu\n", i, p_buffer->content_type, buf_offset, buf_next_offset, len, (unsigned long)p_buffer->next);

        p_buffer_len = buf_next_offset - buf_offset;
        p_buffer->len = p_buffer_len;
        p_buffer_len_no_nl = p_buffer_len;
        p_buf_no_nl = p_buf + p_buffer_len - 1;
        for(int i_no_nl = 0; i_no_nl < 2; i_no_nl++) {
            if(p_buffer_len_no_nl && *p_buf_no_nl && (*p_buf_no_nl == '\r' || *p_buf_no_nl == '\n')) {
                p_buffer_len_no_nl--;
                p_buf_no_nl--;
            }
        }
        p_buffer->len_no_nl = p_buffer_len_no_nl;

        p_buffer->buf = malloc(MAX_TEXTLINE_SIZE + 1);
        memcpy(p_buffer->buf, p_buf, p_buffer_len_no_nl);
        p_buffer->buf[p_buffer_len_no_nl] = 0;

        p_buf = p_next_buf;
        buf_offset = buf_next_offset;
    }
    return error_code;
}

Err
_vedit3_buffer_info_set_buf_from_resource_dict_get_next_buf(char *p_buf, int buf_offset, int len, char **p_next_buf, int *buf_next_offset)
{
    int tmp_next_offset = 0;
    char *tmp_next_buf = NULL;
    for(tmp_next_offset = buf_offset, tmp_next_buf = p_buf; tmp_next_offset < len && *tmp_next_buf != '\n'; tmp_next_offset++, tmp_next_buf++);

    if(tmp_next_offset != len) {
        tmp_next_offset++;
        tmp_next_buf++;
    }

    *p_next_buf = tmp_next_buf;
    *buf_next_offset = tmp_next_offset;

    return S_OK;
}

/**********
 * shrink
 **********/
Err
_sync_vedit3_buffer_info_count_shrink_range(VEdit3BufferInfo *buffer_info, int *n_shrink_range)
{
    *n_shrink_range = buffer_info->n_buffer < N_SHRINK_VEDIT3_BUFFER ? 0 : buffer_info->n_buffer - N_SHRINK_VEDIT3_BUFFER;

    return S_OK;
}

Err
_sync_vedit3_buffer_info_shrink_head(VEdit3BufferInfo *buffer_info, int n_shrink_range)
{
    Err error_code = S_OK;
    VEdit3Buffer *p_orig_head = buffer_info->head;
    VEdit3Buffer *p_buffer = buffer_info->head;

    int i = 0;
    for (i = 0; i < n_shrink_range && p_buffer; i++, p_buffer = p_buffer->next);

    error_code = pttui_thread_lock_wrlock(LOCK_VEDIT3_BUFFER_INFO);
    if (error_code) return error_code;

    buffer_info->head = p_buffer;
    buffer_info->n_buffer -= i;

    if (!p_buffer) { // XXX should not happen
        bzero(buffer_info, sizeof(VEdit3BufferInfo));
    }

    error_code = pttui_thread_lock_unlock(LOCK_VEDIT3_BUFFER_INFO);
    if (error_code) return error_code;

    p_buffer = p_orig_head;
    VEdit3Buffer *tmp = NULL;
    while (p_buffer && p_buffer != buffer_info->head) {
        tmp = p_buffer->next;
        if(p_buffer->buf) free(p_buffer->buf);
        free(p_buffer);
        p_buffer = tmp;
    }

    return S_OK;
}

Err
_sync_vedit3_buffer_info_shrink_tail(VEdit3BufferInfo *buffer_info, int n_shrink_range)
{
    Err error_code = S_OK;
    VEdit3Buffer *p_orig_tail = buffer_info->tail;
    VEdit3Buffer *p_buffer = buffer_info->tail;

    int i = 0;
    for (i = 0; i < n_shrink_range && p_buffer; i++, p_buffer = p_buffer->pre);

    error_code = pttui_thread_lock_wrlock(LOCK_VEDIT3_BUFFER_INFO);
    if (error_code) return error_code;

    buffer_info->tail = p_buffer;
    buffer_info->n_buffer -= i;

    if (!p_buffer) { // XXX should not happen
        bzero(buffer_info, sizeof(VEdit3BufferInfo));
    }

    error_code = pttui_thread_lock_unlock(LOCK_VEDIT3_BUFFER_INFO);
    if (error_code) return error_code;

    p_buffer = p_orig_tail;
    VEdit3Buffer *tmp = NULL;
    while (p_buffer && p_buffer != buffer_info->tail) {
        tmp = p_buffer->pre;
        if(p_buffer->buf) free(p_buffer->buf);
        free(p_buffer);
        p_buffer = tmp;
    }

    return S_OK;
}

Err
vedit3_buffer_insert_buffer(VEdit3Buffer *current_buffer, VEdit3Buffer *next_buffer, VEdit3BufferInfo *buffer_info)
{
    next_buffer->next = current_buffer->next;
    next_buffer->pre = current_buffer;

    current_buffer->next = next_buffer;
    if(next_buffer->next) {
        next_buffer->next->pre = next_buffer;
    }
    else {
        buffer_info->tail = next_buffer;
    }

    return S_OK;
}