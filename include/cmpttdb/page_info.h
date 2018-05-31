/* $Id$ */
#ifndef PAGE_INFO_H
#define PAGE_INFO_H

#include "pttconst.h"
#include "cmpttdb/pttdb_const.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PageInfo {
    aidu_t aid;

    enum PttDBContentType content_type;
    int line_id;

    enum PttDBStorageType storage_type;
} PageInfo;

typedef struct FilePageInfo {
    aidu_t aid;

    int n_page;
    PageInfo *page_infos;
};

Err InitPageInfo();

Err DestroyPageInfo();

#ifdef __cplusplus
}
#endif

#endif /* PAGE_INFO_H */
