#include "cmpttui/vedit3_resource_dict.h"
#include "cmpttui/vedit3_resource_dict_private.h"

Err
vedit3_resource_dict_get_main_from_db(PttQueue *queue, VEdit3ResourceDict *resource_dict)
{
    fprintf(stderr, "vedit3_resource_dict.vedit3_resource_dict_get_main_from_db: n_queue: %d\n", queue->n_queue);

    if(!queue->n_queue) return S_OK;

    VEdit3Buffer *head_buffer = (VEdit3Buffer *)queue->head->val.p;
    int min_block_id = head_buffer->block_offset;

    VEdit3Buffer *tail_buffer = (VEdit3Buffer *)queue->tail->val.p;
    int max_block_id = tail_buffer->block_offset;

    char *_uuid = display_uuid(head_buffer->the_id);
    fprintf(stderr, "vedit3_resource_dict.get_content_block_from_db_core: the_id: %s min_block_id: %d max_block_id: %d\n", _uuid, min_block_id, max_block_id);
    safe_free((void **)&_uuid);

    return _vedit3_resource_dict_get_content_block_from_db_core(head_buffer->the_id, min_block_id, max_block_id, MONGO_MAIN_CONTENT, resource_dict);
}

Err
safe_destroy_vedit3_resource_dict(VEdit3ResourceDict *resource_dict)
{
    _VEdit3ResourceDictLinkList *p = NULL;
    _VEdit3ResourceDictLinkList *tmp = NULL;

    for(int i = 0; i < N_VEDIT3_RESOURCE_DICT_LINK_LIST; i++) {
        p = resource_dict->data[i];

        while(p) {
            tmp = p->next;
            if(p->buf) free(p->buf);
            free(p);
            p = tmp;
        }

        resource_dict->data[i] = NULL;
    }

    return S_OK;
}

Err
_vedit3_resource_dict_content_block_db_to_dict(bson_t **b_content_blocks, int n_content_block, VEdit3ResourceDict *resource_dict)
{
    Err error_code = S_OK;

    bson_t **p_content_block = b_content_blocks;
    int block_id = 0;
    int len_block = 0;
    char *buf_block = NULL;
    int len = 0;
    UUID the_id = {};
    for(int i = 0; i < n_content_block; i++, p_content_block++) {
        error_code = bson_get_value_bin(*p_content_block, "the_id", UUIDLEN, (char *)the_id, &len);
        if(error_code) break;

        error_code = bson_get_value_int32(*p_content_block, "block_id", &block_id);
        if(error_code) break;

        error_code = bson_get_value_int32(*p_content_block, "len_block", &len_block);
        if(error_code) break;

        error_code = bson_get_value_bin_not_initialized(*p_content_block, "buf_block", &buf_block, &len);
        if(error_code) break;

        error_code = _vedit3_resource_dict_add_data(the_id, block_id, len_block, buf_block, resource_dict);
        if(error_code) break;
    }

    return error_code;
}

Err
vedit3_resource_dict_get_comment_from_db(PttQueue *queue, VEdit3ResourceDict *resource_dict)
{
    Err error_code = S_OK;
    if(!queue->n_queue) return S_OK;

    bson_t child = {};
    char buf[16] = {};
    const char *array_key = NULL;
    size_t array_keylen = 0;

    bson_t *q_array = bson_new();

    VEdit3Buffer *p_buffer = NULL;
    BSON_APPEND_ARRAY_BEGIN(q_array, "$in", &child);    
    int i = 0;
    int status = 0;
    PttLinkList *p = queue->head;
    for(; p; p = p->next, i++) {
        p_buffer = (VEdit3Buffer *)p->val.p;
        array_keylen = bson_uint32_to_string(i, &array_key, buf, sizeof(buf));
        status = bson_append_bin(&child, array_key, (int)array_keylen, p_buffer->the_id, UUIDLEN);
        if (!status) {
            error_code = S_ERR;
            break;
        }
    }
    bson_append_array_end(q_array, &child);

    bson_t *q = BCON_NEW(
        "the_id", BCON_DOCUMENT(q_array)
        );

    bson_t *fields = BCON_NEW(
        "_id", BCON_BOOL(false),
        "the_id", BCON_BOOL(true),
        "len", BCON_BOOL(true),
        "buf", BCON_BOOL(true)
        );

    int max_n_comment = queue->n_queue;
    int n_comment = 0;

    bson_t **b_comments = NULL;
    if(max_n_comment) {
        b_comments = malloc(sizeof(bson_t *) * max_n_comment);
        bzero(b_comments, sizeof(bson_t *) * max_n_comment);
    }

    if(!error_code && max_n_comment) {
        error_code = read_comments_by_query_to_bsons(q, fields, max_n_comment, b_comments, &n_comment);
    }

    if(!error_code && n_comment) {
        error_code = _vedit3_resource_dict_comment_db_to_dict(b_comments, n_comment, resource_dict);
    }

    //free
    bson_safe_destroy(&q);
    bson_safe_destroy(&fields);
    bson_safe_destroy(&q_array);
    safe_free_b_list(&b_comments, n_comment);

    return S_OK;
}

Err
_vedit3_resource_dict_comment_db_to_dict(bson_t **b_comments, int n_comment, VEdit3ResourceDict *resource_dict)
{
    Err error_code = S_OK;

    bson_t **p_comment = b_comments;
    int block_id = 0;
    int len_block = 0;
    char *buf_block = NULL;
    int len = 0;
    UUID the_id = {};
    for(int i = 0; i < n_comment; i++, p_comment++) {
        error_code = bson_get_value_bin(*p_comment, "the_id", UUIDLEN, (char *)the_id, &len);
        if(error_code) break;

        error_code = bson_get_value_int32(*p_comment, "len", &len_block);
        if(error_code) break;

        error_code = bson_get_value_bin_not_initialized(*p_comment, "buf", &buf_block, &len);
        if(error_code) break;

        error_code = _vedit3_resource_dict_add_data(the_id, block_id, len_block, buf_block, resource_dict);
        if(error_code) break;
    }

    return error_code;
}

Err
vedit3_resource_dict_get_comment_reply_from_db(PttQueue *queue, VEdit3ResourceDict *resource_dict)
{
    Err error_code = S_OK;
    if(!queue->n_queue) return S_OK;

    bson_t child = {};
    char buf[16] = {};
    const char *array_key = NULL;
    size_t array_keylen = 0;

    VEdit3Buffer *p_pre_buffer = NULL;
    VEdit3Buffer *p_buffer = NULL;
    PttLinkList *p = NULL;

    bson_t *q_array = NULL;

    // init-head
    VEdit3Buffer *p_head_buffer = (VEdit3Buffer *)queue->head->val.p;
    UUID head_uuid = {};
    memcpy(head_uuid, p_head_buffer->the_id, UUIDLEN);

    // init-tail
    VEdit3Buffer *p_tail_buffer = (VEdit3Buffer *)queue->tail->val.p;
    UUID tail_uuid = {};
    memcpy(tail_uuid, p_tail_buffer->the_id, UUIDLEN);

    // head
    p_pre_buffer = p_head_buffer;
    for(p = queue->head; p; p = p->next) {
        p_buffer = (VEdit3Buffer *)p->val.p;
        if(memcmp(head_uuid, p_buffer->the_id, UUIDLEN)) break;
        p_pre_buffer = p_buffer;
    }    

    int min_block_id = p_head_buffer->block_offset;
    int max_block_id = p_pre_buffer->block_offset;

    error_code = _vedit3_resource_dict_get_content_block_from_db_core(head_uuid, min_block_id, max_block_id, MONGO_COMMENT_REPLY_BLOCK, resource_dict);

    if(error_code) return error_code;

    if(!p) return S_OK;

    // middle
    int max_n_middle = 0;
    q_array = bson_new();
    BSON_APPEND_ARRAY_BEGIN(q_array, "$in", &child);
    UUID pre_uuid = {};
    int i = 0;    
    int status = 0;
    for(; p; p = p->next, max_n_middle++, i++) {
        VEdit3Buffer *p_buffer = (VEdit3Buffer *)p->val.p;
        if(!memcmp(p_buffer->the_id, tail_uuid, UUIDLEN)) break;

        if(!memcmp(p_buffer->the_id, pre_uuid, UUIDLEN)) continue;
        memcpy(pre_uuid, p_buffer->the_id, UUIDLEN);

        array_keylen = bson_uint32_to_string(i, &array_key, buf, sizeof(buf));
        status = bson_append_bin(&child, array_key, (int)array_keylen, p_buffer->the_id, UUIDLEN);
        if (!status) {
            error_code = S_ERR;
            break;
        }        
    }
    bson_append_array_end(q_array, &child);

    bson_t *q_middle = NULL;
    if(max_n_middle) {
        q_middle = BCON_NEW(
            "the_id", BCON_DOCUMENT(q_array)
            );
        error_code = _vedit3_resource_dict_get_content_block_from_db_core2(q_middle, max_n_middle, MONGO_COMMENT_REPLY_BLOCK, resource_dict);
    }
    fprintf(stderr, "vedit3_resource_dict.vedit3_resource_dict_get_comment_reply_from_db: after middle: max_n_middle: %d e: %d\n", max_n_middle, error_code);

    // tail
    if(!error_code && p) {
        p_buffer = (VEdit3Buffer *)p->val.p;
        min_block_id = p_buffer->block_offset;
        max_block_id = p_tail_buffer->block_offset;
        error_code = _vedit3_resource_dict_get_content_block_from_db_core(tail_uuid, min_block_id, max_block_id, MONGO_COMMENT_REPLY_BLOCK, resource_dict);
        fprintf(stderr, "vedit3_resource_dict.vedit3_resource_dict_get_comment_reply_from_db: after tail: min_block_id: %d max_block_id: %d e: %d\n", min_block_id, max_block_id, error_code);
    }

    //free
    bson_safe_destroy(&q_array);
    bson_safe_destroy(&q_middle);

    return error_code;
}

Err
_vedit3_resource_dict_add_data(UUID the_id, int block_id, int len, char *buf, VEdit3ResourceDict *resource_dict)
{
    // XXX buf_block need to be freed after copy to vedit3-buffer
    int the_idx = ((int)the_id[0] + block_id) % N_VEDIT3_RESOURCE_DICT_LINK_LIST;
    _VEdit3ResourceDictLinkList *p = resource_dict->data[the_idx];
    if(!p) {
        resource_dict->data[the_idx] = malloc(sizeof(_VEdit3ResourceDictLinkList));
        p = resource_dict->data[the_idx];
    }
    else {
        for(; p->next; p = p->next);

        p->next = malloc(sizeof(_VEdit3ResourceDictLinkList));
        p = p->next;
    }

    p->next = NULL;
    memcpy(p->the_id, the_id, UUIDLEN);
    p->block_id = block_id;
    p->len = len;
    p->buf = buf;

    return S_OK;
}

Err
vedit3_resource_dict_get_data(VEdit3ResourceDict *resource_dict, UUID the_id, int block_id, int *len, char **buf)
{
    int the_idx = ((int)the_id[0] + block_id) % N_VEDIT3_RESOURCE_DICT_LINK_LIST;
    _VEdit3ResourceDictLinkList *p = resource_dict->data[the_idx];
    for(; p; p = p->next) {
        if(!memcmp(the_id, p->the_id, UUIDLEN) && block_id == p->block_id) {
            *len = p->len;
            *buf = p->buf;
            break;
        }
    }

    if(!p) {
        *len = 0;
        *buf = NULL;
        return S_ERR_NOT_EXISTS;
    }

    return S_OK;
}

Err
_vedit3_resource_dict_get_content_block_from_db_core(UUID uuid, int min_block_id, int max_block_id, enum MongoDBId mongo_db_id, VEdit3ResourceDict *resource_dict)
{    
    Err error_code = S_OK;
    bson_t *q = BCON_NEW(
        "the_id", BCON_BINARY(uuid, UUIDLEN),
        "block_id", "{",
            "$gte", BCON_INT32(min_block_id),
            "$lte", BCON_INT32(max_block_id),
        "}"        
        );

    int max_n_content_block = max_block_id - min_block_id + 1;

    error_code = _vedit3_resource_dict_get_content_block_from_db_core2(q, max_n_content_block, mongo_db_id, resource_dict);

    // free

    bson_safe_destroy(&q);

    return error_code;
}

Err
_vedit3_resource_dict_get_content_block_from_db_core2(bson_t *q, int max_n_content_block, enum MongoDBId mongo_db_id, VEdit3ResourceDict *resource_dict)
{
    Err error_code = S_OK;

    bson_t *fields = BCON_NEW(
        "_id", BCON_BOOL(false),
        "the_id", BCON_BOOL(true),
        "block_id", BCON_BOOL(true),
        "len_block", BCON_BOOL(true),
        "buf_block", BCON_BOOL(true)
        );

    bson_t **b_content_blocks = malloc(sizeof(bson_t *) * max_n_content_block);
    bzero(b_content_blocks, sizeof(bson_t *) * max_n_content_block);
    int n_content_block = 0;

    error_code = read_content_blocks_by_query_to_bsons(q, fields, max_n_content_block, mongo_db_id, b_content_blocks, &n_content_block);

    if(!error_code) {
        error_code = _vedit3_resource_dict_content_block_db_to_dict(b_content_blocks, n_content_block, resource_dict);
    }

    // free
    bson_safe_destroy(&fields);
    safe_free_b_list(&b_content_blocks, n_content_block);

    return error_code;
}

Err
vedit3_resource_dict_get_main_from_file(PttQueue *queue, VEdit3ResourceDict *resource_dict)
{
    return S_OK;
}

Err
vedit3_resource_dict_get_comment_from_file(PttQueue *queue, VEdit3ResourceDict *resource_dict)
{
    return S_OK;
}

Err
vedit3_resource_dict_get_comment_reply_from_file(PttQueue *queue, VEdit3ResourceDict *resource_dict)
{
    return S_OK;
}
