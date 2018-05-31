/* $Id$ */
#ifndef PTTDB_CONST_H
#define PTTDB_CONST_H

#ifdef __cplusplus
extern "C" {
#endif

enum PttDBContentType {
    PTTDB_CONTENT_TYPE_MAIN,

    N_PTTDB_CONTENT_TYPE
};

enum PttDBStorageType {
    PTTDB_STORAGE_TYPE_MMAP,

    N_PTTDB_STORAGE_TYPE
};

#ifdef __cplusplus
}
#endif

#endif /* PTTDB_CONST_H */
