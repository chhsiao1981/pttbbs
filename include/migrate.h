/* $Id$ */
#ifndef MIGRATE_H
#define MIGRATE_H

#include "ptterr.h"
#include "pttdb.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct LegacyCommentInfo {
    int comment_offset;

    time64_t comment_create_milli_timestamp;
    char comment_poster[IDLEN + 1];
    char comment_ip[IPV4LEN + 1];
    int comment_len;
    enum CommentType comment_type;

    time64_t comment_reply_create_milli_timestamp;
    char comment_reply_poster[IDLEN + 1];
    char comment_reply_ip[IPV4LEN + 1];
    int comment_reply_len;
} LegacyCommentInfo;

typedef struct LegacyFileInfo {
    aidu_t aid;
    char web_link[MAX_WEB_LINK_LEN + 1];

    char poster[IDLEN + 1];
    char board[IDLEN + 1];
    char title[TTLEN + 1];
    time64_t create_milli_timestamp;
    
    char origin[MAX_ORIGIN_LEN + 1];
    char ip[IPV4LEN + 1];
    int main_content_len;

    int n_comment_comment_reply;
    LegacyCommentInfo *comment_info;
} LegacyFileInfo;

enum LegacyFileStatus {
    LEGACY_FILE_STATUS_MAIN_CONTENT,
    LEGACY_FILE_STATUS_COMMENT,
    LEGACY_FILE_STATUS_COMMENT_REPLY,
    LEGACY_FILE_STATUS_END
};

Err migrate_db_to_file(UUID main_id, const char *fpath);

Err parse_legacy_file(const char *fpath, LegacyFileInfo *legacy_file_info);
Err init_legacy_file_info_comment_comment_reply(LegacyFileInfo *legacy_file_info, int n_comment_comment_reply);
Err safe_destroy_legacy_file_info(LegacyFileInfo *legacy_file_info);

#ifdef __cplusplus
}
#endif

#endif /* MIGRATE_H */
