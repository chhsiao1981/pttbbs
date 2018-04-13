/* $Id$ */
#ifndef VEDIT3_RESOURCE_INFO_H
#define VEDIT3_RESOURCE_INFO_H

#include "ptterr.h"
#include "cmpttdb.h"
#include "cmpttlib.h"
#include "cmpttui/vedit3_resource_dict.h"
#include "cmpttui/vedit3_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct VEdit3ResourceInfo {
    PttQueue queue[N_PTTDB_CONTENT_TYPE * N_PTTDB_STORAGE_TYPE];
} VEdit3ResourceInfo;

Err
vedit3_resource_info_push_queue(VEdit3Buffer *buffer, VEdit3ResourceInfo *resource_info, enum PttDBContentType content_type, enum StorageType storage_type);

Err destroy_vedit3_resource_info(VEdit3ResourceInfo *resource_info);

Err vedit3_resource_info_to_resource_dict(VEdit3ResourceInfo *resource_info, VEdit3ResourceDict *resource_dict);

#ifdef __cplusplus
}
#endif

#endif /* VEDIT3_RESOURCE_INFO_H */
