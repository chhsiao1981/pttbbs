#include "cmpttui/pttui_resource_info.h"

Err
pttui_resource_info_push_queue(PttUIBuffer *buffer, PttUIResourceInfo *resource_info, enum PttDBContentType content_type, enum PttDBStorageType storage_type)
{
    Err error_code = S_OK;

    int queue_idx = content_type * N_PTTDB_STORAGE_TYPE + storage_type;

    PttQueue *queue = &(resource_info->queue[queue_idx]);

    error_code = ptt_enqueue_p(buffer, queue);

    return error_code;
}

Err
destroy_pttui_resource_info(PttUIResourceInfo *resource_info)
{
    PttQueue *p_queue = resource_info->queue;
    for(int i = 0; i < N_PTTDB_CONTENT_TYPE * N_PTTDB_STORAGE_TYPE; i++, p_queue++) {
        destroy_ptt_queue(&resource_info->queue[i]);
    }

    return S_OK;
}

Err
pttui_resource_info_to_resource_dict(PttUIResourceInfo *resource_info, PttUIResourceDict *resource_dict)
{
    Err error_code = S_OK;

    PttQueue *p_queue = &resource_info->queue[PTTDB_CONTENT_TYPE_MAIN * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_MONGO];
    fprintf(stderr, "pttui_resource_info.pttui_resource_info_to_resource_dict: to get main from db: n_queue: %d\n", p_queue->n_queue);
    error_code = pttui_resource_dict_get_main_from_db(p_queue, resource_dict);
    if(error_code) return error_code;

    p_queue = &resource_info->queue[PTTDB_CONTENT_TYPE_MAIN * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_FILE];
    fprintf(stderr, "pttui_resource_info.pttui_resource_info_to_resource_dict: to get main from file: n_queue: %d\n", p_queue->n_queue);
    error_code = pttui_resource_dict_get_main_from_file(p_queue, resource_dict);
    if(error_code) return error_code;

    p_queue = &resource_info->queue[PTTDB_CONTENT_TYPE_COMMENT * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_MONGO];
    fprintf(stderr, "pttui_resource_info.pttui_resource_info_to_resource_dict: to get comment from db: n_queue: %d\n", p_queue->n_queue);
    error_code = pttui_resource_dict_get_comment_from_db(p_queue, resource_dict);
    if(error_code) return error_code;

    p_queue = &resource_info->queue[PTTDB_CONTENT_TYPE_COMMENT * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_FILE];
    fprintf(stderr, "pttui_resource_info.pttui_resource_info_to_resource_dict: to get comment from file: n_queue: %d\n", p_queue->n_queue);
    error_code = pttui_resource_dict_get_comment_from_file(p_queue, resource_dict);
    if(error_code) return error_code;

    p_queue = &resource_info->queue[PTTDB_CONTENT_TYPE_COMMENT_REPLY * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_MONGO];
    fprintf(stderr, "pttui_resource_info.pttui_resource_info_to_resource_dict: to get comment-reply from db: n_queue: %d\n", p_queue->n_queue);
    error_code = pttui_resource_dict_get_comment_reply_from_db(p_queue, resource_dict);
    if(error_code) return error_code;

    p_queue = &resource_info->queue[PTTDB_CONTENT_TYPE_COMMENT_REPLY * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_FILE];
    fprintf(stderr, "pttui_resource_info.pttui_resource_info_to_resource_dict: to get comment-reply from file: n_queue: %d\n", p_queue->n_queue);
    error_code = pttui_resource_dict_get_comment_reply_from_file(p_queue, resource_dict);
    if(error_code) return error_code;

    return S_OK;
}

Err
log_pttui_resource_info(PttUIResourceInfo *resource_info, char *prompt)
{
    char prompt2[MAX_BUF_SIZE] = {};
    PttQueue *p_queue = &resource_info->queue[PTTDB_CONTENT_TYPE_MAIN * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_MONGO];
    sprintf(prompt2, "%s: main-db:", prompt);
    _log_pttui_resource_info_queue(p_queue, prompt2);

    p_queue = &resource_info->queue[PTTDB_CONTENT_TYPE_MAIN * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_FILE];
    sprintf(prompt2, "%s: main-file:", prompt);
    _log_pttui_resource_info_queue(p_queue, prompt2);

    p_queue = &resource_info->queue[PTTDB_CONTENT_TYPE_COMMENT * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_MONGO];
    sprintf(prompt2, "%s: comment-db:", prompt);
    _log_pttui_resource_info_queue(p_queue, prompt2);

    p_queue = &resource_info->queue[PTTDB_CONTENT_TYPE_COMMENT * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_FILE];
    sprintf(prompt2, "%s: comment-file:", prompt);
    _log_pttui_resource_info_queue(p_queue, prompt2);

    p_queue = &resource_info->queue[PTTDB_CONTENT_TYPE_COMMENT_REPLY * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_MONGO];
    sprintf(prompt2, "%s: comment-reply-db:", prompt);
    _log_pttui_resource_info_queue(p_queue, prompt2);

    p_queue = &resource_info->queue[PTTDB_CONTENT_TYPE_COMMENT_REPLY * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_FILE];
    sprintf(prompt2, "%s: comment-reply-file:", prompt);
    _log_pttui_resource_info_queue(p_queue, prompt2);

    return S_OK;
}

Err
_log_pttui_resource_info_queue(PttQueue *q, char *prompt)
{
    PttLinkList *p = q->head;
    int i = 0;
    PttUIBuffer *p_buffer;
    for(; p; p = p->next, i++) {
        p_buffer = (PttUIBuffer *)p->val.p;
        fprintf(stderr, "%s: (%d/%d) content_type: %d comment: %d block: %d line: %d file: %d\n", prompt, i, p->n_queue, p_buffer->content_type, p_buffer->comment_offset, p_buffer->block_offset, p_buffer->line_offset, p_buffer->file_offset);
    }

    return S_OK;
}