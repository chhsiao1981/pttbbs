/* $Id$ */
#ifndef PTTDB_FILE_INFO_H
#define PTTDB_FILE_INFO_H

#include "ptterr.h"
#include "cmpttdb/pttdb_const.h"
#include "cmpttdb/pttdb_uuid.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "pttstruct.h"

typedef struct ContentBlockInfo {
    int n_line;
    enum StorageType storage_type;
} ContentBlockInfo;

typedef struct CommentInfo {
    UUID comment_id;

    char comment_poster[IDLEN + 1];
    time64_t comment_create_milli_timestamp;

    UUID comment_reply_id;
    int n_comment_reply_total_line;
    int n_comment_reply_block;
    ContentBlockInfo *comment_reply_blocks;
} CommentInfo;

typedef struct FileInfo {
    UUID main_id;
    char main_poster[IDLEN + 1];
    time64_t main_create_milli_timestamp;
    time64_t main_update_milli_timestamp;

    UUID main_content_id;

    int n_main_line;
    int n_main_block;
    ContentBlockInfo *main_blocks;

    int n_comment;
    CommentInfo *comments;

} FileInfo;

Err construct_file_info(UUID main_id, FileInfo *file_info);
Err destroy_file_info(FileInfo *file_info);

#ifdef __cplusplus
}
#endif

#endif /* PTTDB_FILE_INFO_H */

