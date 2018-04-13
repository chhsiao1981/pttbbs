/* $Id$ */
#ifndef VEDIT3_RESOURCE_DICT_H
#define VEDIT3_RESOURCE_DICT_H

#include "ptterr.h"
#include "cmpttlib.h"
#include "cmpttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define N_VEDIT3_RESOURCE_DICT_LINK_LIST 256

typedef struct _VEdit3ResourceDictLinkList {
    struct _VEdit3ResourceDictLinkList *next;
    UUID the_id;
    int block_id;
    int len;
    char *buf;
} _VEdit3ResourceDictLinkList;

typedef struct VEdit3ResourceDict {
    _VEdit3ResourceDictLinkList *data[N_VEDIT3_RESOURCE_DICT_LINK_LIST];
} VEdit3ResourceDict;

Err safe_destroy_vedit3_resource_dict(VEdit3ResourceDict *resource_dict);

Err vedit3_resource_dict_get_data(VEdit3ResourceDict *resource_dict, UUID the_id, int block_id, int *len, char **buf);

Err vedit3_resource_dict_get_main_from_db(PttQueue *queue, VEdit3ResourceDict *resource_dict);

Err vedit3_resource_dict_get_comment_from_db(PttQueue *queue, VEdit3ResourceDict *resource_dict);

Err vedit3_resource_dict_get_comment_reply_from_db(PttQueue *queue, VEdit3ResourceDict *resource_dict);

Err vedit3_resource_dict_get_main_from_file(PttQueue *queue, VEdit3ResourceDict *resource_dict);

Err vedit3_resource_dict_get_comment_from_file(PttQueue *queue, VEdit3ResourceDict *resource_dict);

Err vedit3_resource_dict_get_comment_reply_from_file(PttQueue *queue, VEdit3ResourceDict *resource_dict);

#ifdef __cplusplus
}
#endif

#endif /* VEDIT3_RESOURCE_DICT_H */

