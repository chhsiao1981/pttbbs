#include "migrate.h"
#include "migrate_internal.h"

Err
migrate_file_to_db(const char *fpath, UUID main_id)
{
    Err error_code = S_OK;
    LegacyFileInfo legacy_file_info = {};

    error_code = parse_legacy_file(fpath, &legacy_file_info);

    int fd = open(fpath, O_RDONLY);

    UUID main_id = {};
    UUID content_id = {};

    if(!error_code) {
        error_code = create_main_from_fd(legacy_file_info.aid, legacy_file_info.board, legacy_file_info.title, legacy_file_info.poster, legacy_file_info.ip, legacy_file_info.origin, legacy_file_info.web_link, legacy_file_info.main_content_len, fd, main_id, content_id);
    }

    UUID comment_id = {};
    UUID comment_reply_id = {};

    char buf[MAX_MIGRATE_COMMENT_COMMENT_REPLY_BUF_SIZE];
    if(!error_code) {
        for(int i = 0; i < legacy_file_info.n_comment_comment_reply; i++) {
            lseek(fd, legacy_file_info.comment_info[i].comment_offset, SEEK_SET);
            read(fd, buf, legacy_file_info.comment_info[i].comment_len);
            error_code = create_comment(
                main_id,
                legacy_file_info.comment_info[i].comment_poster,
                legacy_file_info.comment_info[i].comment_ip,
                legacy_file_info.comment_info[i].comment_len,
                buf,
                legacy_file_info.comment_info[i].comment_type,
                comment_id
                );
            if(error_code) break;

            // comment-reply
            if(!legacy_file_info.comment_info[i].comment_reply_len) continue;

            read(fd, buf, legacy_file_info.comment_info[i].comment_reply_len);
            error_code = create_comment_reply(
                main_id,
                comment_id,
                legacy_file_info.comment_info[i].comment_reply_poster,
                legacy_file_info.comment_info[i].comment_reply_ip,
                legacy_file_info.comment_info[i].comment_reply_len,
                buf,
                comment_reply_id
                )
            if(error_code) break;
        }
    }

    //free
    close(fd);

    safe_destroy_legacy_file_info(&legacy_file_info);

    return error_code;
}


// parse state
// start => first-2-lines => main-content => origin => comment => comment-reply => white-lines
//                                                          <=======
Err
parse_legacy_file(const char *fpath, LegacyFileInfo *legacy_file_info)
{
    Err error_code = S_OK;

    error_code = _parse_legacy_file_main_info(fpath, legacy_file_info);

    if(!error_code) {
        error_code = _parse_legacy_file_comment_comment_reply(fpath, legacy_file_info);
    }

    return error_code;
}


Err
_parse_legacy_file_comment_comment_reply(const char *fpath, LegacyFileInfo *legacy_file_info)
{
    Err error_code = S_OK;

    error_code = _parse_legacy_file_n_comment_comment_reply(fpath, legacy_file_info->main_content_len, &n_comment_comment_reply);

    if(!error_code) {
        error_code = init_legacy_file_info_comment_comment_reply(legacy_file_info, n_comment_comment_reply);
}

Err
safe_destroy_legacy_file_info(LegacyFileInfo *legacy_file_info)
{
    safe_free(&legacy_file_info->comment_info);
    legacy_file_info->n_comment_comment_reply = 0;

    return S_OK;
}