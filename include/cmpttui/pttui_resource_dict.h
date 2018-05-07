/* $Id$ */
#ifndef PTTUI_RESOURCE_DICT_H
#define PTTUI_RESOURCE_DICT_H

#include "ptterr.h"
#include "cmpttlib.h"
#include "cmpttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define N_PTTUI_RESOURCE_DICT_LINK_LIST 256

typedef struct _PttUIResourceDictLinkList {
    struct _PttUIResourceDictLinkList *next;
    UUID the_id;
    int block_id;
    int len;
    char *buf;
} _PttUIResourceDictLinkList;

typedef struct PttUIResourceDict {
    _PttUIResourceDictLinkList *data[N_PTTUI_RESOURCE_DICT_LINK_LIST];
} PttUIResourceDict;

Err safe_destroy_pttui_resource_dict(PttUIResourceDict *resource_dict);

Err pttui_resource_dict_get_data(PttUIResourceDict *resource_dict, UUID the_id, int block_id, int *len, char **buf);

Err pttui_resource_dict_get_main_from_db(PttQueue *queue, PttUIResourceDict *resource_dict);

Err pttui_resource_dict_get_comment_from_db(PttQueue *queue, PttUIResourceDict *resource_dict);

Err pttui_resource_dict_get_comment_reply_from_db(PttQueue *queue, PttUIResourceDict *resource_dict);

Err pttui_resource_dict_get_main_from_file(PttQueue *queue, PttUIResourceDict *resource_dict);

Err pttui_resource_dict_get_comment_from_file(PttQueue *queue, PttUIResourceDict *resource_dict);

Err pttui_resource_dict_get_comment_reply_from_file(PttQueue *queue, PttUIResourceDict *resource_dict);

Err log_pttui_resource_dict(PttUIResourceDict *resource_dict, char *prompt);

#ifdef __cplusplus
}
#endif

#endif /* PTTUI_RESOURCE_DICT_H */

