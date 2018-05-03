#include "cmpttui/vedit3_buffer.h"
#include "cmpttui/vedit3_buffer_private.h"

Err
destroy_vedit3_buffer_info(VEdit3BufferInfo *buffer_info)
{
    VEdit3Buffer *p_buffer = buffer_info->head;
    VEdit3Buffer *tmp = NULL;
    while (p_buffer != NULL) {
        tmp = p_buffer;
        p_buffer = p_buffer->next;
        if(tmp->buf) free(tmp->buf);
        free(tmp);
    }

    bzero(buffer_info, sizeof(VEdit3Buffer));

    return S_OK;
}

Err
vedit3_buffer_is_begin_of_file(VEdit3Buffer *buffer, FileInfo *file_info, bool *is_begin)
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

Err
vedit3_buffer_split(VEdit3Buffer *buffer, int offset, int indent, VEdit3Buffer *new_buffer)
{
    Err error_code = S_OK;
    
    return error_code;
}

Err
sync_vedit3_buffer_info(VEdit3BufferInfo *buffer_info, VEdit3Buffer *current_buffer, VEdit3State *state, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    Err error_code = S_OK;

    // no buffer in buffer_info
    if (!current_buffer) {
        return resync_all_vedit3_buffer_info(buffer_info, state, file_info, new_buffer);
    }

    // determine new buffer of the expected-state
    bool is_pre = false;
    error_code = _sync_vedit3_buffer_info_is_pre(state, current_buffer, &is_pre);
    if (error_code) return error_code;

    fprintf(stderr, "vedit3_buffer.sync_vedit3_buffer_info: expected_state: content_type: %d block_offset: %d line_offset: %d comment_offset: %d buffer: content_type: %d block_offset: %d line_offset: %d comment_offset: %d is_pre: %d\n", state->top_line_content_type, state->top_line_block_offset, state->top_line_line_offset, state->top_line_comment_offset, current_buffer->content_type, current_buffer->block_offset, current_buffer->line_offset, current_buffer->comment_offset, is_pre);

    error_code = _sync_vedit3_buffer_info_get_buffer(state, current_buffer, is_pre, new_buffer);

    if (error_code) return error_code;

    fprintf(stderr, "vedit3_buffer.sync_vedit3_buffer_info: after get buffer: new_buffer: %lu\n", new_buffer);

    if (!(*new_buffer)) {
        return resync_all_vedit3_buffer_info(buffer_info, state, file_info, new_buffer);
    }

    // 1. determine extra-range required to retrieve.
    // 2. do extend buffer.
    // 3. determine range to shrink.
    // 4. shrink range.
    int n_extra_range = 0;
    int n_shrink_range = 0;
    if (is_pre) {
        error_code = _sync_vedit3_buffer_info_count_extra_pre_range(*new_buffer, &n_extra_range);
        if (error_code) return error_code;

        if (n_extra_range) {
            error_code = _sync_vedit3_buffer_info_extend_pre_buffer(buffer_info, state, file_info, n_extra_range);
        }
        if (error_code) return error_code;

        error_code = _sync_vedit3_buffer_info_count_shrink_range(buffer_info, &n_shrink_range);
        if (error_code) return error_code;

        if (n_shrink_range) {
            error_code = _sync_vedit3_buffer_info_shrink_tail(buffer_info, n_shrink_range);
        }
    }
    else {
        error_code = _sync_vedit3_buffer_info_count_extra_next_range(*new_buffer, &n_extra_range);
        if (error_code) return error_code;

        fprintf(stderr, "vedit3_buffer.sync_vedit3_buffer_info: after count extra next range: n_extra_range: %d\n", n_extra_range);

        if (n_extra_range) {
            error_code = _sync_vedit3_buffer_info_extend_next_buffer(buffer_info, state, file_info, n_extra_range);
        }
        if (error_code) return error_code;

        fprintf(stderr, "vedit3_buffer.sync_vedit3_buffer_info: after extend next buffer\n");

        error_code = _sync_vedit3_buffer_info_count_shrink_range(buffer_info, &n_shrink_range);
        if (error_code) return error_code;

        fprintf(stderr, "vedit3_buffer.sync_vedit3_buffer_info: after count shrink range: n_shrink_range: %d\n", n_shrink_range);

        if (n_shrink_range) {
            error_code = _sync_vedit3_buffer_info_shrink_head(buffer_info, n_shrink_range);
        }

        fprintf(stderr, "vedit3_buffer.sync_vedit3_buffer_info: after shrink range\n");
    }

    return error_code;
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

    /**********
     * both are not main
     **********/
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
_construct_vedit3_buffer_from_vedit3_state_with_file_info(VEdit3State *state, FileInfo *file_info, VEdit3Buffer **buffer)
{
    *buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *p_buffer = *buffer;
    bzero(p_buffer, sizeof(VEdit3Buffer));

    memcpy(p_buffer->the_id, state->top_line_id, UUIDLEN);
    p_buffer->content_type = state->top_line_content_type;
    p_buffer->block_offset = state->top_line_block_offset;
    p_buffer->line_offset = state->top_line_line_offset;
    p_buffer->comment_offset = state->top_line_comment_offset;

    CommentInfo *p_comment_info = NULL;
    ContentBlockInfo *p_content_block_info = NULL;

    switch (p_buffer->content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        p_content_block_info = file_info->main_blocks + p_buffer->block_offset;
        p_buffer->storage_type = p_content_block_info->storage_type;
        break;
    case PTTDB_CONTENT_TYPE_COMMENT:
        p_buffer->storage_type = PTTDB_STORAGE_TYPE_MONGO;
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        p_comment_info = file_info->comments + p_buffer->comment_offset;
        p_content_block_info = p_comment_info->comment_reply_blocks + p_buffer->block_offset;
        p_buffer->storage_type = p_content_block_info->storage_type;
        break;
    default:
        break;
    }

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

Err
resync_all_vedit3_buffer_info(VEdit3BufferInfo *buffer_info, VEdit3State *state, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    Err error_code = pttui_thread_lock_wrlock(LOCK_VEDIT3_BUFFER_INFO);
    fprintf(stderr, "vedit3_buffer.resync_all_vedit3_buffer_info: after wrlock: e: %d\n", error_code);
    if (error_code) return error_code;

    error_code = destroy_vedit3_buffer_info(buffer_info);
    fprintf(stderr, "vedit3_buffer.resync_all_vedit3_buffer_info: after destroy_vedit3_buffer_info: e: %d\n", error_code);
    if (error_code) return error_code;

    error_code = pttui_thread_lock_unlock(LOCK_VEDIT3_BUFFER_INFO);
    fprintf(stderr, "vedit3_buffer.resync_all_vedit3_buffer_info: after unlock: e: %d\n", error_code);
    if (error_code) return error_code;

    error_code = _sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info(state, file_info, buffer_info);
    fprintf(stderr, "vedit3_buffer.resync_all_vedit3_buffer_info: after init buffer no buf from file-info: e: %d\n", error_code);
    if (error_code) return error_code;

    *new_buffer = buffer_info->head;

    error_code = _sync_vedit3_buffer_info_extend_pre_buffer(buffer_info, state, file_info, HARD_N_VEDIT3_BUFFER);
    fprintf(stderr, "vedit3_buffer.resync_all_vedit3_buffer_info: after extend pre buffer: e: %d\n", error_code);
    if (error_code) return error_code;

    error_code = _sync_vedit3_buffer_info_extend_next_buffer(buffer_info, state, file_info, HARD_N_VEDIT3_BUFFER);
    fprintf(stderr, "vedit3_buffer.resync_all_vedit3_buffer_info: after extend next buffer: e: %d\n", error_code);
    if (error_code) return error_code;

    return S_OK;
}

Err
_sync_vedit3_buffer_info_count_extra_pre_range(VEdit3Buffer *buffer, int *n_extra_range)
{
    VEdit3Buffer *p_buffer = NULL;
    int i = 0;
    for (i = 0, p_buffer = buffer; i < SOFT_N_VEDIT3_BUFFER && p_buffer; i++, p_buffer = p_buffer->pre);

    *n_extra_range = i == SOFT_N_VEDIT3_BUFFER ? 0 : (HARD_N_VEDIT3_BUFFER - i);

    return S_OK;
}

Err
_sync_vedit3_buffer_info_count_extra_next_range(VEdit3Buffer *buffer, int *n_extra_range)
{
    VEdit3Buffer *p_buffer = NULL;
    // XXX log
    int total_i = 0;
    for(total_i = 0, p_buffer = buffer; p_buffer; total_i++, p_buffer = p_buffer->next);

    int i = 0;
    for (i = 0, p_buffer = buffer; i < SOFT_N_VEDIT3_BUFFER && p_buffer; i++, p_buffer = p_buffer->next);

    *n_extra_range = i == SOFT_N_VEDIT3_BUFFER ? 0 : (HARD_N_VEDIT3_BUFFER - i);

    fprintf(stderr, "vedit3_buffer._sync_vedit3_buffer_info_count-extra_next_range: total_i: %d i: %d n_extra_range: %d\n", total_i, i, *n_extra_range);

    return S_OK;
}

/**********
 * extend pre buffer
 **********/
Err
_sync_vedit3_buffer_info_extend_pre_buffer(VEdit3BufferInfo *buffer_info, VEdit3State *state, FileInfo *file_info, int n_buffer)
{
    Err error_code = S_OK;
    if (!buffer_info->head) {
        error_code = _sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info(state, file_info, buffer_info);
        if (error_code) return error_code;
        n_buffer--;
    }

    VEdit3Buffer *start_buffer = buffer_info->head;

    bool is_begin = false;
    if(start_buffer && start_buffer->buf) {
        error_code = vedit3_buffer_is_begin_of_file(start_buffer, file_info, &is_begin);
        if(error_code) return error_code;
        if(is_begin) return S_OK;
    }

    VEdit3ResourceInfo resource_info = {};
    VEdit3ResourceDict resource_dict = {};
    error_code = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf(buffer_info, file_info, n_buffer);
    if (error_code) return error_code;

    // XXX log
    int total_i = 0;
    VEdit3Buffer *p_buffer = start_buffer;
    for(total_i = 0; p_buffer; total_i++, p_buffer = p_buffer->pre);
    fprintf(stderr, "vedit3_buffer._sync_vedit3_buffer_info_extend_pre_buffer: n_buffer: %d total_i (from buffer_info->head): %d\n", n_buffer, total_i);

    if (start_buffer->buf) start_buffer = start_buffer->pre;

    fprintf(stderr, "vedit3_buffer._sync_vedit3_buffer_info_extend_pre_buffer: to buffer_info_to_resource_info\n");
    error_code = _vedit3_buffer_info_to_resource_info(start_buffer, &resource_info);

    fprintf(stderr, "vedit3_buffer._sync_vedit3_buffer_info_extend_pre_buffer: after buffer_info to resource_info: e: %d\n", error_code);
    if (!error_code) {
        error_code = vedit3_resource_info_to_resource_dict(&resource_info, &resource_dict);
    }

    fprintf(stderr, "vedit3_buffer._sync_vedit3_buffer_info_extend_pre_buffer: after resource_info to resource_dict: e: %d buffer_info: %d\n", error_code, buffer_info->n_buffer);
    if (!error_code) {
        error_code = _vedit3_buffer_info_set_buf_from_resource_dict(start_buffer, &resource_dict);
    }

    fprintf(stderr, "vedit3_buffer._sync_vedit3_buffer_info_extend_pre_buffer: after set buf from resource_dict: e: %d\n", error_code);
    // free
    destroy_vedit3_resource_info(&resource_info);
    safe_destroy_vedit3_resource_dict(&resource_dict);

    return error_code;
}

Err
_sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info(VEdit3State *state, FileInfo *file_info, VEdit3BufferInfo *buffer_info)
{
    VEdit3Buffer *buffer = NULL;
    Err error_code = _construct_vedit3_buffer_from_vedit3_state_with_file_info(state, file_info, &buffer);
    if (error_code) return error_code;
    if (!buffer) return S_ERR_NOT_EXISTS;

    buffer_info->head = buffer;
    buffer_info->tail = buffer;
    buffer_info->n_buffer = 1;
    memcpy(buffer_info->main_id, state->main_id, UUIDLEN);

    return S_OK;
}

Err
_sync_vedit3_buffer_info_extend_pre_buffer_no_buf(VEdit3BufferInfo *buffer_info, FileInfo *file_info, int n_buffer)
{
    Err error_code = S_OK;
    VEdit3Buffer *current_buffer = buffer_info->head;
    VEdit3Buffer *new_buffer = NULL;
    int i = 0;
    for (i = 0; i < n_buffer; i++) {
        error_code = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_core(current_buffer, file_info, &new_buffer);
        if (error_code) break;

        if (!new_buffer) break;

        new_buffer->next = current_buffer;
        current_buffer->pre = new_buffer;
        buffer_info->head = new_buffer;
        current_buffer = new_buffer;
        new_buffer = NULL;
    }
    buffer_info->n_buffer += i;

    return error_code;
}

Err
_sync_vedit3_buffer_info_extend_pre_buffer_no_buf_core(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    Err error_code = S_OK;
    switch (current_buffer->content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        error_code = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_main(current_buffer, file_info, new_buffer);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT:
        error_code = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment(current_buffer, file_info, new_buffer);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        error_code = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment_reply(current_buffer, file_info, new_buffer);
        break;
    default:
        error_code = S_ERR;
        break;
    }

    return error_code;
}

Err
_sync_vedit3_buffer_info_extend_pre_buffer_no_buf_main(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    if (current_buffer->block_offset == 0 && current_buffer->line_offset == 0) return S_OK;

    *new_buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *tmp = *new_buffer;
    bzero(tmp, sizeof(VEdit3Buffer));

    memcpy(tmp->the_id, current_buffer->the_id, UUIDLEN);
    tmp->content_type = current_buffer->content_type;
    tmp->comment_offset = 0;

    if (current_buffer->line_offset != 0) {
        tmp->block_offset = current_buffer->block_offset;
        tmp->line_offset = current_buffer->line_offset - 1;
        tmp->storage_type = current_buffer->storage_type;
    }
    else {
        tmp->block_offset = current_buffer->block_offset - 1;
        tmp->line_offset = file_info->main_blocks[tmp->block_offset].n_line - 1;
        tmp->storage_type = file_info->main_blocks[tmp->block_offset].storage_type;
    }

    return S_OK;
}

Err
_sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    *new_buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *tmp = *new_buffer;
    bzero(tmp, sizeof(VEdit3Buffer));

    tmp->storage_type = PTTDB_STORAGE_TYPE_MONGO;

    CommentInfo *p_comment = NULL;
    ContentBlockInfo *p_comment_reply_block;
    if (current_buffer->comment_offset == 0) {
        tmp->content_type = PTTDB_CONTENT_TYPE_MAIN;
        memcpy(tmp->the_id, file_info->main_content_id, UUIDLEN);
        tmp->block_offset = file_info->n_main_block - 1;
        tmp->line_offset = file_info->main_blocks[tmp->block_offset].n_line - 1;
        tmp->comment_offset = 0;
    }
    else {
        tmp->comment_offset = current_buffer->comment_offset - 1;
        p_comment = file_info->comments + tmp->comment_offset;
        if (p_comment->n_comment_reply_total_line) {
            tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
            memcpy(tmp->the_id, p_comment->comment_reply_id, UUIDLEN);
            tmp->block_offset = p_comment->n_comment_reply_block - 1;
            p_comment_reply_block = p_comment->comment_reply_blocks + tmp->block_offset;
            tmp->line_offset = p_comment_reply_block->n_line - 1;
            tmp->storage_type = p_comment_reply_block->storage_type;
        }
        else {
            tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT;
            memcpy(tmp->the_id, p_comment->comment_id, UUIDLEN);
            tmp->block_offset = 0;
            tmp->line_offset = 0;
        }
    }

    return S_OK;
}

Err
_sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment_reply(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    *new_buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *tmp = *new_buffer;
    bzero(tmp, sizeof(VEdit3Buffer));

    tmp->comment_offset = current_buffer->comment_offset;

    CommentInfo *p_comment = file_info->comments + tmp->comment_offset;
    ContentBlockInfo *p_comment_reply_block = NULL;
    if (current_buffer->block_offset == 0 && current_buffer->line_offset == 0) {
        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT;
        memcpy(tmp->the_id, p_comment->comment_id, UUIDLEN);
        tmp->block_offset = 0;
        tmp->line_offset = 0;
        tmp->storage_type = PTTDB_STORAGE_TYPE_MONGO;
    }
    else {
        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
        memcpy(tmp->the_id, current_buffer->the_id, UUIDLEN);

        if (current_buffer->line_offset != 0) {
            tmp->block_offset = current_buffer->block_offset;
            tmp->line_offset = current_buffer->line_offset - 1;
            tmp->storage_type = current_buffer->storage_type;
        }
        else {
            tmp->block_offset = current_buffer->block_offset - 1;
            p_comment_reply_block = p_comment->comment_reply_blocks + tmp->block_offset;
            tmp->line_offset = p_comment_reply_block->n_line - 1;
            tmp->storage_type = p_comment_reply_block->storage_type;
        }
    }

    return S_OK;
}

/**********
 * extend next buffer
 **********/
Err
_sync_vedit3_buffer_info_extend_next_buffer(VEdit3BufferInfo *buffer_info, VEdit3State *state, FileInfo *file_info, int n_buffer)
{
    Err error_code = S_OK;
    if (!buffer_info->tail) {
        error_code = _sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info(state, file_info, buffer_info);
        if (error_code) return error_code;
        n_buffer--;
    }

    VEdit3Buffer *start_buffer = buffer_info->tail;

    bool is_eof = false;
    if (start_buffer && start_buffer->buf) {
        error_code = vedit3_buffer_is_eof(start_buffer, file_info, &is_eof);
        if (error_code) return error_code;
        if (is_eof) return S_OK;
    }

    VEdit3ResourceInfo resource_info = {};
    VEdit3ResourceDict resource_dict = {};
    error_code = _sync_vedit3_buffer_info_extend_next_buffer_no_buf(buffer_info, file_info, n_buffer);
    if (error_code) return error_code;

    // XXX log
    int total_i = 0;
    VEdit3Buffer *p_buffer = start_buffer;
    for(total_i = 0; p_buffer; total_i++, p_buffer = p_buffer->next);
    fprintf(stderr, "vedit3_buffer._sync_vedit3_buffer_info_extend_next_buffer: n_buffer: %d total_i (from buffer_info->tail): %d\n", n_buffer, total_i);

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
_sync_vedit3_buffer_info_extend_next_buffer_no_buf(VEdit3BufferInfo *buffer_info, FileInfo *file_info, int n_buffer)
{
    Err error_code = S_OK;
    VEdit3Buffer *current_buffer = buffer_info->tail;
    VEdit3Buffer *new_buffer = NULL;
    int i = 0;
    for (i = 0; i < n_buffer; i++) {
        error_code = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_core(current_buffer, file_info, &new_buffer);
        if (error_code) break;

        if (!new_buffer) break;

        new_buffer->pre = current_buffer;
        current_buffer->next = new_buffer;
        buffer_info->tail = new_buffer;
        current_buffer = new_buffer;
        new_buffer = NULL;
    }
    buffer_info->n_buffer += i;

    return error_code;
}

Err
_sync_vedit3_buffer_info_extend_next_buffer_no_buf_core(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    Err error_code = S_OK;
    switch (current_buffer->content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        error_code = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_main(current_buffer, file_info, new_buffer);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT:
        error_code = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment(current_buffer, file_info, new_buffer);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        error_code = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment_reply(current_buffer, file_info, new_buffer);
        break;
    default:
        error_code = S_ERR;
        break;
    }

    return error_code;
}

Err
_sync_vedit3_buffer_info_extend_next_buffer_no_buf_main(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    if (current_buffer->block_offset == file_info->n_main_block - 1 &&
            current_buffer->line_offset == file_info->main_blocks[current_buffer->block_offset].n_line - 1 &&
            !file_info->n_comment) return S_OK;

    *new_buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *tmp = *new_buffer;
    bzero(tmp, sizeof(VEdit3Buffer));

    // last line of main-block. new-buffer as comment
    if (current_buffer->block_offset == file_info->n_main_block - 1 &&
            current_buffer->line_offset == file_info->main_blocks[current_buffer->block_offset].n_line - 1) {

        memcpy(tmp->the_id, file_info->comments[0].comment_id, UUIDLEN);
        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT;
        tmp->comment_offset = 0;
        tmp->block_offset = 0;
        tmp->line_offset = 0;
        tmp->storage_type = PTTDB_STORAGE_TYPE_MONGO;

        return S_OK;
    }

    // still in main-block
    memcpy(tmp->the_id, current_buffer->the_id, UUIDLEN);
    tmp->content_type = current_buffer->content_type;
    tmp->comment_offset = 0;

    if (current_buffer->line_offset != file_info->main_blocks[current_buffer->block_offset].n_line - 1) {
        // not the last-line
        tmp->block_offset = current_buffer->block_offset;
        tmp->line_offset = current_buffer->line_offset + 1;
        tmp->storage_type = current_buffer->storage_type;
    }
    else {
        // last-line, but not the last block
        tmp->block_offset = current_buffer->block_offset + 1;
        tmp->line_offset = 0;
        tmp->storage_type = file_info->main_blocks[tmp->block_offset].storage_type;
    }

    return S_OK;
}

Err
_sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    int current_buffer_comment_offset = current_buffer->comment_offset;
    CommentInfo *p_comment = file_info->comments + current_buffer_comment_offset;
    ContentBlockInfo *p_comment_reply_block = NULL;

    // the end of file
    if (current_buffer_comment_offset == file_info->n_comment - 1 &&
            !p_comment->n_comment_reply_block) return S_OK;

    *new_buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *tmp = *new_buffer;
    bzero(tmp, sizeof(VEdit3Buffer));

    if (p_comment->n_comment_reply_block) {
        // with comment-reply
        p_comment_reply_block = p_comment->comment_reply_blocks;

        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
        memcpy(tmp->the_id, p_comment->comment_reply_id, UUIDLEN);
        tmp->comment_offset = current_buffer_comment_offset;
        tmp->block_offset = 0;
        tmp->line_offset = 0;
        tmp->storage_type = p_comment_reply_block->storage_type;
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
    }

    return S_OK;
}

Err
_sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment_reply(VEdit3Buffer *current_buffer, FileInfo *file_info, VEdit3Buffer **new_buffer)
{
    int current_buffer_comment_offset = current_buffer->comment_offset;
    int current_buffer_block_offset = current_buffer->block_offset;
    int current_buffer_line_offset = current_buffer->line_offset;

    CommentInfo *p_comment = file_info->comments + current_buffer_comment_offset;
    ContentBlockInfo *p_comment_reply_block = p_comment->comment_reply_blocks + current_buffer_block_offset;

    // the end of file
    if (current_buffer_comment_offset == file_info->n_comment - 1 &&
            current_buffer_block_offset == p_comment->n_comment_reply_block - 1 &&
            current_buffer_line_offset == p_comment_reply_block->n_line - 1) return S_OK;

    *new_buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *tmp = *new_buffer;
    bzero(tmp, sizeof(VEdit3Buffer));

    if (current_buffer_line_offset != p_comment_reply_block->n_line - 1) {
        // within the same block
        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
        memcpy(tmp->the_id, p_comment->comment_reply_id, UUIDLEN);
        tmp->comment_offset = current_buffer_comment_offset;
        tmp->block_offset = current_buffer_block_offset;
        tmp->line_offset = current_buffer_line_offset + 1;

        tmp->storage_type = p_comment_reply_block->storage_type;
    }
    else if (current_buffer_block_offset != p_comment->n_comment_reply_block - 1) {
        // different block, within same comment
        tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
        memcpy(tmp->the_id, p_comment->comment_reply_id, UUIDLEN);
        tmp->comment_offset = current_buffer_comment_offset;
        tmp->block_offset = current_buffer_block_offset + 1;
        tmp->line_offset = 0;

        p_comment_reply_block++;
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

    // pre-head
    error_code = vedit3_resource_dict_get_data(resource_dict, p_buffer->the_id, p_buffer->block_offset, &len, &buf);
    if(error_code) return error_code;

    memcpy(current_the_id, p_buffer->the_id, UUIDLEN);
    current_block_id = p_buffer->block_offset;
    p_buf = buf;
    buf_offset = 0;

    for(int i = 0; i < p_buffer->line_offset; i++) {
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
            fprintf(stderr, "vedit3_buffer._vedit3_buffer_info_set_buf_from_resource_dict: after get data: i: %d len: %d e: %d\n", i, len, error_code);

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

        fprintf(stderr, "vedit3_buffer._vedit3_buffer_info_set_buf_from_resource_dict: i: %d content_type: %d buf_offset: %d buf_next_offset: %d len: %d p_next: %lu\n", i, p_buffer->content_type, buf_offset, buf_next_offset, len, p_buffer->next);

        p_buffer_len = buf_next_offset - buf_offset;
        p_buffer->len = p_buffer_len;
        p_buffer_len_no_nl = p_buffer_len;
        p_buf_no_nl = buf_next_offset - 1;
        for(int i_no_nl = 0; i_no_nl < 2; i_no_nl++) {
            if(p_buffer_len_no_nl) {
                if(*p_buf_no_nl && (*p_buf_no_nl == '\r' || *p_buf_no_nl == '\n')) {
                    p_buffer_len_no_nl--;
                    p_buf_no_nl--;
                }
            }
        }
        p_buffer->len_no_nl = p_buffer_len_no_nl;

        p_buffer->buf = malloc(p_buffer_len + 1);
        memcpy(p_buffer->buf, p_buf, p_buffer_len);
        p_buffer->buf[p_buffer_len] = 0;

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
