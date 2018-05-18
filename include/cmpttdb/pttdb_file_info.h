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
    int n_line_in_db;
    int n_new_line;
    int n_to_delete_line;
    enum PttDBStorageType storage_type;

    int n_file;
    int *file_n_line;
} ContentBlockInfo;

typedef struct CommentInfo {
    UUID comment_id;

    char comment_poster[IDLEN + 1];
    time64_t comment_create_milli_timestamp;

    enum PttDBStorageType storage_type;

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

    int n_total_line;

} FileInfo;

Err construct_file_info(UUID main_id, FileInfo *file_info);
Err destroy_file_info(FileInfo *file_info);

bool file_info_is_first_block_ne(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset);

Err file_info_is_first_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, bool *is_first_block);

bool file_info_is_last_block_ne(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset);

Err file_info_is_last_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, bool *is_last_block);

Err file_info_pre_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, int *new_block_offset);

Err file_info_next_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, int *new_block_offset);

Err file_info_first_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int *new_block_offset);

Err file_info_last_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int *new_block_offset);

bool file_info_is_first_line_ne(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, int line_offset);

Err file_info_is_first_line(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, int line_offset, bool *is_first_line);

Err file_info_is_last_line(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, int line_offset, bool *is_last_line);

Err file_info_get_content_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int block_offset, ContentBlockInfo **content_block);

Err file_info_get_n_block(FileInfo *file_info, enum PttDBContentType content_type, int comment_offset, int *n_block);

Err file_info_get_pre_line(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type);

Err file_info_get_next_line(FileInfo *file_info, UUID orig_id, enum PttDBContentType orig_content_type, int orig_block_offset, int orig_line_offset, int orig_comment_offset, UUID new_id, enum PttDBContentType *new_content_type, int *new_block_offset, int *new_line_offset, int *new_comment_offset, enum PttDBStorageType *new_storage_type);

Err file_info_increase_main_content_line(FileInfo *file_info, int block_offset, int file_offset);

Err file_info_decrease_main_content_line(FileInfo *file_info, int block_offset, int file_offset);

Err file_info_update_main_content_storage_type(FileInfo *file_info, int block_offset, enum PttDBStorageType storage_type);

Err file_info_update_comment_storage_type(FileInfo *file_info, int comment_offset, enum PttDBStorageType storage_type);

Err file_info_increase_comment_reply_line(FileInfo *file_info, int comment_offset, int block_offset, int file_offset);

Err file_info_decrease_comment_reply_line(FileInfo *file_info, int comment_offset, int block_offset, int file_offset);

Err file_info_update_comment_reply_storage_type(FileInfo *file_info, int comment_offset, int block_offset, enum PttDBStorageType storage_type);

Err save_file_info_to_db(FileInfo *file_info, char *user, char *ip);

Err log_file_info(FileInfo *file_info, char *prompt);

#ifdef __cplusplus
}
#endif

#endif /* PTTDB_FILE_INFO_H */

