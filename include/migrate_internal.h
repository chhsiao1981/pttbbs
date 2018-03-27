/* $Id$ */
#ifndef MIGRATE_INTERNAL_H
#define MIGRATE_INTERNAL_H

#include "pttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define COMMENT_BUF_SIZE 80
#define N_MIGRATE_MAIN_CONTENT_TO_FILE_BLOCK 5
#define N_MIGRATE_COMMENT_COMMENT_REPLY_TO_FILE_BLOCK 256
#define MAX_MIGRATE_COMMENT_COMMENT_REPLY_BUF_SIZE 8192 * 5

Err _parse_create_milli_timestamp_from_filename(char *filename, time64_t *create_milli_timestamp);
Err _parse_create_milli_timestamp_from_web_link(char *web_link, time64_t *create_milli_timestamp);

/*****
 * main-info
 *****/
Err _parse_legacy_file_main_info(const char *fpath, LegacyFileInfo *legacy_file_info);
Err _parse_legacy_file_main_info_core(char *buf, int bytes, char *line, int *bytes_in_line, LegacyFileInfo *legacy_file_info, enum LegacyFileStatus *status);
Err _parse_legacy_file_main_info_core_one_line(char *line, int bytes_in_line, LegacyFileInfo *legacy_file_info, enum LegacyFileStatus *status);
Err _parse_legacy_file_main_info_core_one_line_main_content(char *line, int bytes_in_line, LegacyFileInfo *legacy_file_info, enum LegacyFileStatus *status);
Err _parse_legacy_file_main_info_last_line(int bytes_in_line, char *line, LegacyFileInfo *legacy_file_info, enum LegacyFileStatus *status);


/*****
 * comment comment-reply
 *****/
Err _parse_legacy_file_comment_comment_reply(const char *fpath, LegacyFileInfo *legacy_file_info);

// n comment comment-reply
Err _parse_legacy_file_n_comment_comment_reply(const char *fpath, int main_content_len, int *n_comment_comment_reply);
Err _parse_legacy_file_n_comment_comment_reply_core(char *buf, int bytes, char *line, int *bytes_in_line, int *n_comment_comment_reply);
Err _parse_legacy_file_n_comment_comment_reply_core_one_line(char *line, int bytes_in_line, int *n_comment_comment_reply);
Err _parse_legacy_file_n_comment_comment_reply_last_line(int bytes_in_line, char *line, int *n_comment_comment_reply);

// comment comment-reply-core
Err _parse_legacy_file_comment_comment_reply_core(const char *fpath, LegacyFileInfo *legacy_file_info);
Err _parse_legacy_file_comment_comment_reply_core_core(char *buf, int bytes, char *line, int *bytes_in_line, LegacyFileInfo *legacy_file_info, int *comment_idx, time64_t *current_create_milli_timestamp, enum LegacyFileStatus *status);
Err _parse_legacy_file_comment_comment_reply_core_one_line(char *line, int bytes_in_line, LegacyFileInfo *legacy_file_info, int *comment_idx, time64_t *current_create_milli_timestamp, enum LegacyFileStatus *status);
Err _parse_legacy_file_comment_comment_reply_core_one_line_comment(char *line, int bytes_in_line, LegacyFileInfo *legacy_file_info, int *comment_idx, time64_t *current_create_milli_timestamp, enum LegacyFileStatus *status);
Err _parse_legacy_file_comment_comment_reply_core_one_line_comment_reply(char *line, int bytes_in_line, LegacyFileInfo *legacy_file_info, int *comment_idx, enum LegacyFileStatus *status);
Err _parse_legacy_file_comment_comment_reply_core_last_line(int bytes_in_line, char *line, LegacyFileInfo *legacy_file_info, int *comment_idx, time64_t *current_create_milli_timestamp, enum LegacyFileStatus *status);


Err _parse_legacy_file_comment_create_milli_timestamp(char *line, int bytes_in_line, time64_t current_create_milli_timestamp, time64_t *create_milli_timestamp);
Err
_parse_legacy_file_comment_create_milli_timestamp_good_bad_arrow_cross(char *line, int bytes_in_line, time64_t current_create_milli_timestamp, time64_t *create_milli_timestamp);

Err _parse_legacy_file_comment_poster(char *line, int bytes_in_line, char *poster);
Err _parse_legacy_file_comment_poster_cross(char *line, int bytes_in_line, char *poster);
Err _parse_legacy_file_comment_poster_good_bad_arrow(char *line, int bytes_in_line, char *poster);

Err _parse_legacy_file_comment_type(char *line, int bytes_in_line, enum CommentType *comment_type);

// is-comment-line
Err _is_comment_line(char *line, int bytes_in_line, bool *is_valid);
Err _is_comment_line_good_bad_arrow(char *line, int bytes_in_line, bool *is_valid, enum CommentType comment_type);
Err _is_comment_line_cross(char *line, int bytes_in_line, bool *is_valid);

// migrate db to file
Err _migrate_main_content_to_file(MainHeader *main_header, FILE *fp);
Err _migrate_main_content_to_file_core(UUID content_id, FILE *fp, int start_block_id, int next_block_id);
Err _migrate_comment_comment_reply_by_main_to_file(UUID main_id, FILE *fp);
Err _migrate_comment_comment_reply_by_main_to_file_core(bson_t **b_comments, int n_comment, FILE *fp);

#ifdef __cplusplus
}
#endif

#endif /* MIGRATE_INTERNAL_H */
