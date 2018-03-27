#include "migrate.h"
#include "migrate_internal.h"

int
get_web_url(const boardheader_t *bp, const fileheader_t *fhdr, char *buf,
          size_t szbuf)
{
    const char *folder = bp->brdname, *fn = fhdr->filename, *ext = ".html";

/*
#ifdef USE_AID_URL
    char aidc[32] = "";
    aidu_t aidu = fn2aidu(fhdr->filename);
    if (!aidu)
    return 0;

    aidu2aidc(aidc, aidu);
    fn = aidc;
    ext = "";
#endif
*/

    if (!fhdr || !*fhdr->filename || *fhdr->filename == 'L' ||
        *fhdr->filename == '.')
    return 0;

    return snprintf(buf, szbuf, URL_PREFIX "/%s/%s%s", folder, fn, ext);
}

Err
migrate_get_file_info(const fileheader_t *fhdr, const boardheader_t *bp, char *poster, char *board, char *title, char *origin, aidu_t *aid, char *web_link, time64_t *create_milli_timestamp)
{
    memcpy(poster, fhdr->owner, strlen(fhdr->owner) - 1);
    strcpy(board, bp->brdname);
    strcpy(title, fhdr->title);
    strcpy(origin, "ptt.cc");
    *aid = fn2aidu((char *)fhdr->filename);

    bool status = get_web_url(bp, fhdr, web_link, MAX_WEB_LINK_LEN);
    if(!status) bzero(web_link, MAX_WEB_LINK_LEN);

    Err error_code = _parse_create_milli_timestamp_from_filename((char *)fhdr->filename, create_milli_timestamp);

    return error_code;
}

Err
migrate_file_to_db(const char *fpath, char *poster, char *board, char *title, char *origin, aidu_t aid, char *web_link, UUID main_id)
{
    Err error_code = S_OK;
    LegacyFileInfo legacy_file_info = {};

    // basic info
    strcpy(legacy_file_info.poster, poster);
    strcpy(legacy_file_info.board, board);
    strcpy(legacy_file_info.title, title);
    strcpy(legacy_file_info.origin, origin);
    legacy_file_info.aid = aid;
    strcpy(legacy_file_info.web_link, web_link);    
    error_code = _parse_create_milli_timestamp_from_web_link(web_link, &legacy_file_info.create_milli_timestamp);

    // parse
    if(!error_code) {
        error_code = parse_legacy_file(fpath, &legacy_file_info);
    }

    int fd = open(fpath, O_RDONLY);

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
                );
            if(error_code) break;
        }
    }

    //free
    close(fd);

    safe_destroy_legacy_file_info(&legacy_file_info);

    return error_code;
}

Err
_parse_create_milli_timestamp_from_filename(char *filename, time64_t *create_milli_timestamp)
{
    char *p_filename_milli_timestamp = filename + 2;
    long create_timestamp = atol(p_filename_milli_timestamp);
    if(!create_timestamp) return S_ERR;

    *create_milli_timestamp = create_timestamp * 1000;

    return S_OK;
}

Err
_parse_create_milli_timestamp_from_web_link(char *web_link, time64_t *create_milli_timestamp)
{
    char *p_web_link = web_link + strlen(web_link) - LEN_WEBLINK_POSTFIX - 1 - LEN_AID_POSTFIX - 1 - LEN_AID_INFIX - 1 - LEN_AID_TIMESTAMP;
    int create_timestamp = atoi(p_web_link);
    if(create_timestamp < MIN_CREATE_TIMESTAMP) return S_ERR;

    *create_milli_timestamp = (time64_t)create_timestamp * 1000;

    return S_OK;
}


/**
 * @brief [brief description]
 * @details [long description]
 *          parse state
 *          main-content => comment => [comment-reply] => end
 *                               <=======
 *                                                                         
 *          Can we get poster / board / title / create_milli_timestamp / origin / ip from some place?
 * @param fpath [description]
 * @param legacy_file_info [description]
 */
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

/*****
 * Parse main info
 *****/

Err
_parse_legacy_file_main_info(const char *fpath, LegacyFileInfo *legacy_file_info)
{
    Err error_code = S_OK;

    int bytes = 0;
    char buf[MAX_BUF_SIZE] = {};
    int bytes_in_line = 0;
    char line[MAX_BUF_SIZE] = {};

    int fd = open(fpath, O_RDONLY);

    enum LegacyFileStatus status = LEGACY_FILE_STATUS_MAIN_CONTENT;
    while((bytes = read(fd, buf, MAX_BUF_SIZE)) > 0) {
        error_code = _parse_legacy_file_main_info_core(buf, bytes, line, &bytes_in_line, legacy_file_info, &status);
        if(error_code) break;        

        if(status == LEGACY_FILE_STATUS_COMMENT) break;
    }
    fprintf(stderr, "migrate_file_to_db._parse_legacy_file_main_info: after while: e: %d status: %d bytes_in_line: %d line: %s\n", error_code, status, bytes_in_line, line);

    if(!error_code && bytes_in_line && status != LEGACY_FILE_STATUS_COMMENT) {        
        error_code = _parse_legacy_file_main_info_last_line(bytes_in_line, line, legacy_file_info, &status);
    }

    // hack for check for empty last line (assuming empty in the end)
    if(bytes <= 0 && legacy_file_info->main_content_len >= 2) {
        legacy_file_info->main_content_len -= 2;
    }

    // free
    close(fd);

    return error_code;
}

Err
_parse_legacy_file_main_info_core(char *buf, int bytes, char *line, int *bytes_in_line, LegacyFileInfo *legacy_file_info, enum LegacyFileStatus *status)
{
    Err error_code = S_OK;
    int bytes_in_new_line = 0;
    for(int offset_buf = 0; offset_buf < bytes; offset_buf += bytes_in_new_line) {
        error_code = get_line_from_buf(buf, offset_buf, bytes, line, *bytes_in_line, &bytes_in_new_line);
        *bytes_in_line += bytes_in_new_line;
        if(error_code) {
            error_code = S_OK;
            break;
        }

        error_code = _parse_legacy_file_main_info_core_one_line(line, *bytes_in_line, legacy_file_info, status);

        if(error_code) break;

        if(*status == LEGACY_FILE_STATUS_COMMENT) break;

        // reset line
        bzero(line, *bytes_in_line);
        *bytes_in_line = 0;
    }

    return error_code;
}

/**
 * @brief status
 * @details 
 * 
 * @param line [description]
 * @param bytes_in_line [description]
 * @param legacy_file_info [description]
 * @param LEGACY_FILE_STATUS [description]
 */
Err
_parse_legacy_file_main_info_core_one_line(char *line, int bytes_in_line, LegacyFileInfo *legacy_file_info, enum LegacyFileStatus *status)
{
    Err error_code = S_OK;

    // XXX special treat for comment-line
    bool is_comment_line;
    error_code = _is_comment_line(line, bytes_in_line, &is_comment_line);
    if(error_code) return error_code;    
    if(is_comment_line) {
        *status = LEGACY_FILE_STATUS_COMMENT;
        return S_OK;
    }

    // status
    switch(*status) {
    case LEGACY_FILE_STATUS_MAIN_CONTENT:
        error_code = _parse_legacy_file_main_info_core_one_line_main_content(line, bytes_in_line, legacy_file_info, status);
        break;
    default:
        break;
    }

    return error_code;
}

Err
_parse_legacy_file_main_info_core_one_line_main_content(char *line, int bytes_in_line, LegacyFileInfo *legacy_file_info, enum LegacyFileStatus *status)
{
    Err error_code = S_OK;

    bool is_comment_line = false;
    error_code = _is_comment_line(line, bytes_in_line, &is_comment_line);
    if(error_code) {
        *status = LEGACY_FILE_STATUS_ERROR;
        return S_ERR;
    }

    if(is_comment_line) {
        *status = LEGACY_FILE_STATUS_COMMENT;
        return S_OK;
    }

    // set main-content-len
    legacy_file_info->main_content_len += bytes_in_line;
    fprintf(stderr, "migrate_file_to_db._parse_legacy_file_main_info_core_one_line_main_content: line: %s bytes_in_line: %d main_content_len: %d\n", line, bytes_in_line, legacy_file_info->main_content_len);

    return S_OK;
}

Err
_parse_legacy_file_main_info_last_line(int bytes_in_line, char *line, LegacyFileInfo *legacy_file_info, enum LegacyFileStatus *status)
{
    return _parse_legacy_file_main_info_core_one_line(line, bytes_in_line, legacy_file_info, status);
}

Err
_parse_legacy_file_comment_comment_reply(const char *fpath, LegacyFileInfo *legacy_file_info)
{
    Err error_code = S_OK;

    int n_comment_comment_reply = 0;    
    error_code = _parse_legacy_file_n_comment_comment_reply(fpath, legacy_file_info->main_content_len, &n_comment_comment_reply);

    if(!error_code && n_comment_comment_reply) {
        error_code = init_legacy_file_info_comment_comment_reply(legacy_file_info, n_comment_comment_reply);
    }

    if(!error_code && n_comment_comment_reply) {
        _parse_legacy_file_comment_comment_reply_core(fpath, legacy_file_info);
    }

    //free
    safe_destroy_legacy_file_info(legacy_file_info);

    return error_code;
}

/*****
 * Parse n Comment Comment-Reply
 *****/

Err
_parse_legacy_file_n_comment_comment_reply(const char *fpath, int main_content_len, int *n_comment_comment_reply)
{
    Err error_code = S_OK;

    int tmp_n_comment_comment_reply = 0;
    int each_n_comment_comment_reply = 0;
    int bytes = 0;
    char buf[MAX_BUF_SIZE] = {};
    int bytes_in_line = 0;
    char line[MAX_BUF_SIZE] = {};

    int fd = open(fpath, O_RDONLY);
    lseek(fd, main_content_len, SEEK_SET);
    while((bytes = read(fd, buf, MAX_BUF_SIZE)) > 0) {
        each_n_comment_comment_reply = 0;
        error_code = _parse_legacy_file_n_comment_comment_reply_core(buf, bytes, line, &bytes_in_line, &each_n_comment_comment_reply);
        if(error_code) break;

        tmp_n_comment_comment_reply += each_n_comment_comment_reply;        
    }

    fprintf(stderr, "migrate_file_to_db._parse_legacy_file_n_comment_comment_reply: after while: e: %d bytes_in_line: %d tmp_n_comment_reply: %d\n", error_code, bytes_in_line, tmp_n_comment_comment_reply);

    if(!error_code && bytes_in_line) {
        each_n_comment_comment_reply = 0;
        error_code = _parse_legacy_file_n_comment_comment_reply_last_line(bytes_in_line, line, &each_n_comment_comment_reply);
        if(!error_code) {
            tmp_n_comment_comment_reply += each_n_comment_comment_reply;
        }
        fprintf(stderr, "migrate_file_to_db._parse_legacy_file_n_comment_comment_reply after last line: e: %d bytes_in_line: %d each_n_comment_comment_reply: %d\n", error_code, bytes_in_line, each_n_comment_comment_reply);
    }

    *n_comment_comment_reply = tmp_n_comment_comment_reply;

    // free
    close(fd);

    return error_code;
}


Err
_parse_legacy_file_n_comment_comment_reply_core(char *buf, int bytes, char *line, int *bytes_in_line, int *n_comment_comment_reply)
{
    Err error_code = S_OK;
    int bytes_in_new_line = 0;
    for(int offset_buf = 0; offset_buf < bytes; offset_buf += bytes_in_new_line) {
        error_code = get_line_from_buf(buf, offset_buf, bytes, line, *bytes_in_line, &bytes_in_new_line);
        *bytes_in_line += bytes_in_new_line;
        if(error_code) {
            error_code = S_OK;
            break;
        }

        error_code = _parse_legacy_file_n_comment_comment_reply_core_one_line(line, *bytes_in_line, n_comment_comment_reply);

        if(error_code) break;

        // reset line
        bzero(line, *bytes_in_line);
        *bytes_in_line = 0;
    }

    return error_code;
}

Err
_parse_legacy_file_n_comment_comment_reply_core_one_line(char *line, int bytes_in_line, int *n_comment_comment_reply)
{
    Err error_code = S_OK;
    bool is_comment_line = false;

    error_code = _is_comment_line(line, bytes_in_line, &is_comment_line);
    if(error_code) return error_code;

    if(is_comment_line) {
        (*n_comment_comment_reply)++;
    }

    fprintf(stderr, "migrate_file_to_db._parse_legacy_file_n_comment_comment_reply_core_one_line: bytes_in_line: %d line: %s e: %d is_comment_line: %d n_comment_comment_reply: %d\n", bytes_in_line, line, error_code, is_comment_line, *n_comment_comment_reply);

    return S_OK;
}

Err
_parse_legacy_file_n_comment_comment_reply_last_line(int bytes_in_line, char *line, int *n_comment_comment_reply)
{
    return _parse_legacy_file_n_comment_comment_reply_core_one_line(line, bytes_in_line, n_comment_comment_reply);
}


/*****
 * Parse Comment Comment-Reply Core
 *****/
Err
_parse_legacy_file_comment_comment_reply_core(const char *fpath, LegacyFileInfo *legacy_file_info)
{
    Err error_code = S_OK;

    int bytes = 0;
    char buf[MAX_BUF_SIZE] = {};
    int bytes_in_line = 0;
    char line[MAX_BUF_SIZE] = {};

    int fd = open(fpath, O_RDONLY);
    lseek(fd, legacy_file_info->main_content_len, SEEK_SET);

    enum LegacyFileStatus status = LEGACY_FILE_STATUS_COMMENT;
    int comment_idx = 0;

    time64_t current_create_milli_timestamp = legacy_file_info->create_milli_timestamp;
    while((bytes = read(fd, buf, MAX_BUF_SIZE)) > 0) {
        error_code = _parse_legacy_file_comment_comment_reply_core_core(buf, bytes, line, &bytes_in_line, legacy_file_info, &comment_idx, &current_create_milli_timestamp, &status);
        if(error_code) break;
    }

    if(!error_code && bytes_in_line) {
        error_code = _parse_legacy_file_comment_comment_reply_core_last_line(bytes_in_line, line, legacy_file_info, &comment_idx, &current_create_milli_timestamp, &status);
    }

    // hack for check for empty last line (assuming empty line in the end)
    if(bytes <= 0 && legacy_file_info->comment_info[comment_idx].comment_reply_len >= 2) {
        legacy_file_info->comment_info[comment_idx].comment_reply_len -= 2;
    }

    // free
    close(fd);

    return error_code;    
}

Err
_parse_legacy_file_comment_comment_reply_core_core(char *buf, int bytes, char *line, int *bytes_in_line, LegacyFileInfo *legacy_file_info, int *comment_idx, time64_t *current_create_milli_timestamp, enum LegacyFileStatus *status)
{
    Err error_code = S_OK;
    int bytes_in_new_line = 0;
    for(int offset_buf = 0; offset_buf < bytes; offset_buf += bytes_in_new_line) {
        error_code = get_line_from_buf(buf, offset_buf, bytes, line, *bytes_in_line, &bytes_in_new_line);
        *bytes_in_line += bytes_in_new_line;
        if(error_code) {
            error_code = S_OK;
            break;
        }

        error_code = _parse_legacy_file_comment_comment_reply_core_one_line(line, *bytes_in_line, legacy_file_info, comment_idx, current_create_milli_timestamp, status);

        if(error_code) break;

        // reset line
        bzero(line, *bytes_in_line);
        *bytes_in_line = 0;
    }

    return error_code;
}

Err
_parse_legacy_file_comment_comment_reply_core_one_line(char *line, int bytes_in_line, LegacyFileInfo *legacy_file_info, int *comment_idx, time64_t *current_create_milli_timestamp, enum LegacyFileStatus *status)
{
    Err error_code = S_OK;

    // hack for state-transition in comment-reply -> comment
    switch(*status) {
    case LEGACY_FILE_STATUS_COMMENT:
        error_code = _parse_legacy_file_comment_comment_reply_core_one_line_comment(line, bytes_in_line, legacy_file_info, comment_idx, current_create_milli_timestamp, status);
        break;
    case LEGACY_FILE_STATUS_COMMENT_REPLY:
        error_code = _parse_legacy_file_comment_comment_reply_core_one_line_comment_reply(line, bytes_in_line, legacy_file_info, comment_idx, status);
        if(!error_code && *status == LEGACY_FILE_STATUS_COMMENT) {
            error_code = _parse_legacy_file_comment_comment_reply_core_one_line_comment(line, bytes_in_line, legacy_file_info, comment_idx, current_create_milli_timestamp, status);
        }
        break;
    default:
        break;
    }

    return error_code;
}

Err
_parse_legacy_file_comment_comment_reply_core_one_line_comment(char *line, int bytes_in_line, LegacyFileInfo *legacy_file_info, int *comment_idx, time64_t *current_create_milli_timestamp, enum LegacyFileStatus *status)
{
    Err error_code = S_OK;

    int tmp_comment_idx = *comment_idx;
    // comment-create-milli-timestamp
    error_code = _parse_legacy_file_comment_create_milli_timestamp(line, bytes_in_line, *current_create_milli_timestamp, &legacy_file_info->comment_info[tmp_comment_idx].comment_create_milli_timestamp);
    if(error_code) return error_code;

    // comment-poster
    error_code = _parse_legacy_file_comment_poster(line, bytes_in_line, legacy_file_info->comment_info[tmp_comment_idx].comment_poster);
    if(error_code) return error_code;

    // comment-len
    legacy_file_info->comment_info[tmp_comment_idx].comment_len = bytes_in_line;

    // comment-type
    error_code = _parse_legacy_file_comment_type(line, bytes_in_line, &legacy_file_info->comment_info[tmp_comment_idx].comment_type);
    if(error_code) return error_code;

    // comment-reply-create-milli-timestamp
    legacy_file_info->comment_info[tmp_comment_idx].comment_reply_create_milli_timestamp = legacy_file_info->comment_info[tmp_comment_idx].comment_create_milli_timestamp + 1;

    // comment-reply-poster
    strcpy(legacy_file_info->comment_info[tmp_comment_idx].comment_reply_poster, legacy_file_info->poster);

    // set status
    *status = LEGACY_FILE_STATUS_COMMENT_REPLY;
    *current_create_milli_timestamp = legacy_file_info->comment_info[tmp_comment_idx].comment_create_milli_timestamp;

    return S_OK;
}

/**
 * @brief 
 * @details The purpose here is construct the milli-timestamp that is:
 *          1. the same date in line.
 *          2. larger than current_create_milli_timestamp
 *          
 *          method:
 *          1. get the date from line.
 *          2. get the year from current_create_milli_timestamp.
 *          3. try to construct milli_timestamp.
 *          4. if new milli_timestamp is larger than current_create_milli_timestamp:
 *             year += 1
 *             reconstruct milli_timestamp
 * 
 * @param line [description]
 * @param bytes_in_line [description]
 * @param current_create_milli_timestamp [description]
 * @param create_milli_timestamp [description]
 */
Err
_parse_legacy_file_comment_create_milli_timestamp(char *line, int bytes_in_line, time64_t current_create_milli_timestamp, time64_t *create_milli_timestamp)
{
    Err error_code = S_OK;

    // comment-line-reset
    /*
    bool is_valid = false;
     error_code = _is_comment_line_cross(line, bytes_in_line, &is_valid);
    if(error_code) return error_code;

    if(is_valid) {
       error_code = _parse_legacy_file_comment_create_milli_timestamp_cross(line, bytes_in_line, current_create_milli_timestamp, create_milli_timestamp);
       return error_code;
    }
    */

    // the rest (good / bad / arrow / cross)
    error_code = _parse_legacy_file_comment_create_milli_timestamp_good_bad_arrow_cross(line, bytes_in_line, current_create_milli_timestamp, create_milli_timestamp);

    return error_code;
}

Err
_parse_legacy_file_comment_create_milli_timestamp_good_bad_arrow_cross(char *line, int bytes_in_line, time64_t current_create_milli_timestamp, time64_t *create_milli_timestamp)
{
    Err error_code = S_OK;

    int year = 0;
    time64_t current_create_timestamp = 0;
    error_code = milli_timestamp_to_year(current_create_milli_timestamp, &year);
    if(error_code) return error_code;

    error_code = milli_timestamp_to_timestamp(current_create_milli_timestamp, &current_create_timestamp);
    if(error_code) return error_code;

    int mm = 0;
    int dd = 0;
    int HH = 0;
    int MM = 0;
    error_code = _parse_legacy_file_comment_create_milli_timestamp_get_datetime_from_line(line, bytes_in_line, &mm, &dd, &HH, &MM);
    if(error_code) return error_code;

    time64_t tmp_timestamp = 0;
    error_code = datetime_to_timestamp(year, mm, dd, HH, MM, 0, &tmp_timestamp);
    if(error_code) return error_code;

    if(tmp_timestamp > current_create_timestamp) {
        *create_milli_timestamp = tmp_timestamp * 1000;
        return S_OK;
    }

    if(tmp_timestamp == current_create_timestamp) {
        *create_milli_timestamp = current_create_milli_timestamp + 1;
        return S_OK;
    }

    error_code = datetime_to_timestamp(year + 1, mm, dd, HH, MM, 0, &tmp_timestamp);
    if(error_code) return error_code;

    *create_milli_timestamp = tmp_timestamp * 1000;
    return S_OK;
}

/**
 * @brief [brief description]
 * @details in format mm/dd HH:MM\r\n in the end of the line
 * 
 * @param line [description]
 * @param bytes_in_line [description]
 * @param mm [description]
 * @param dd [description]
 * @param HH [description]
 * @param MM [description]
 */
Err
_parse_legacy_file_comment_create_milli_timestamp_get_datetime_from_line(char *line, int bytes_in_line, int *mm, int *dd, int *HH, int *MM)
{
    if(bytes_in_line < LEN_COMMENT_DATETIME_IN_LINE) return S_ERR;
    char *p_line = line + bytes_in_line - LEN_COMMENT_DATETIME_IN_LINE;

    // mm
    int tmp_mm = atoi(p_line);
    fprintf(stderr, "migrate_file_to_db._parse_legacy_file_comment_create_milli_timestamp_get_datetime_from_line: after mm: p_line: %s tmp_mm: %d\n", p_line, tmp_mm);
    if(tmp_mm < 1 || tmp_mm > 12) return S_ERR;

    // /
    p_line += 2;
    if(*p_line != '/') return S_ERR;

    // dd
    p_line++;
    int tmp_dd = atoi(p_line);
    if(tmp_dd < 1 || tmp_dd > 31) return S_ERR;

    // [space]
    p_line += 2;
    if(*p_line != ' ') return S_ERR;

    // HH
    p_line++;
    int tmp_HH = atoi(p_line);
    if(tmp_HH < 0 || tmp_HH > 23) return S_ERR;

    // :
    p_line += 2;
    if(*p_line != ':') return S_ERR;

    // MM
    p_line++;
    int tmp_MM = atoi(p_line);
    if(tmp_MM < 0 || tmp_MM > 59) return S_ERR;

    // \r\n
    p_line += 2;
    if(*p_line != '\r' || *(p_line + 1) != '\n') return S_ERR;

    *mm = tmp_mm;
    *dd = tmp_dd;
    *HH = tmp_HH;
    *MM = tmp_MM;

    return S_OK;
}

Err
_parse_legacy_file_comment_poster(char *line, int bytes_in_line, char *poster)
{
    Err error_code = S_OK;

    // comment-line-cross
    bool is_valid = false;
    error_code = _is_comment_line_cross(line, bytes_in_line, &is_valid);
    if(error_code) return error_code;

    if(is_valid) {
       error_code = _parse_legacy_file_comment_poster_cross(line, bytes_in_line, poster);
       return error_code;
    }

    // comment-line-reset
    error_code = _is_comment_line_reset(line, bytes_in_line, &is_valid);
    if(error_code) return error_code;

    if(is_valid) {
       error_code = _parse_legacy_file_comment_poster_reset(line, bytes_in_line, poster);
       return error_code;
    }

    // the rest (good / bad / arrow)
    error_code = _parse_legacy_file_comment_poster_good_bad_arrow(line, bytes_in_line, poster);

    return error_code;
}

Err
_parse_legacy_file_comment_poster_cross(char *line, int bytes_in_line, char *poster)
{
    char *p_line = line + LEN_COMMENT_CROSS_HEADER;
    char *p_line2 = p_line;
    for(int i = LEN_COMMENT_CROSS_HEADER; *p_line2 && *p_line2 != ':' && i < bytes_in_line; i++, p_line2++);
    int len_poster = p_line2 - p_line - LEN_COMMENT_CROSS_PREFIX_COLOR;
    if(len_poster < 0) return S_ERR;

    memcpy(poster, p_line, len_poster);

    return S_OK;
}

Err
_parse_legacy_file_comment_poster_reset(char *line, int bytes_in_line, char *poster)
{
    char *p_line = line + 2;
    char *p_line2 = p_line;
    for(int i = 2; *p_line2 && *p_line2 != ' ' && i < bytes_in_line; i++, p_line2++);
    int len_poster = p_line2 - p_line;
    if(len_poster < 0) return S_ERR;

    memcpy(poster, p_line, len_poster);

    return S_OK;
}

Err
_parse_legacy_file_comment_poster_good_bad_arrow(char *line, int bytes_in_line, char *poster)
{
    char *p_line = line + LEN_COMMENT_HEADER;
    char *p_line2 = p_line;
    for(int i = LEN_COMMENT_HEADER; *p_line2 && *p_line2 != ':' && i < bytes_in_line; i++, p_line2++);
    int len_poster = p_line2 - p_line - LEN_COMMENT_PREFIX_COLOR;
    if(len_poster < 0) return S_ERR;

    memcpy(poster, p_line, len_poster);

    return S_OK;
}

Err
_parse_legacy_file_comment_type(char *line, int bytes_in_line, enum CommentType *comment_type)
{
    Err error = S_OK;

    bool is_valid = false;
    error = _is_comment_line_good_bad_arrow(line, bytes_in_line, &is_valid, COMMENT_TYPE_GOOD);
    if(error) return error;
    if(is_valid) {
        *comment_type = COMMENT_TYPE_GOOD;
        return S_OK;
    }

    error = _is_comment_line_good_bad_arrow(line, bytes_in_line, &is_valid, COMMENT_TYPE_BAD);
    if(error) return error;
    if(is_valid) {
        *comment_type = COMMENT_TYPE_BAD;
        return S_OK;
    }

    error = _is_comment_line_good_bad_arrow(line, bytes_in_line, &is_valid, COMMENT_TYPE_ARROW);
    if(error) return error;
    if(is_valid) {
        *comment_type = COMMENT_TYPE_ARROW;
        return S_OK;
    }

    error = _is_comment_line_cross(line, bytes_in_line, &is_valid);
    if(error) return error;
    if(is_valid) {
        *comment_type = COMMENT_TYPE_CROSS;
        return S_OK;
    }


    error = _is_comment_line_reset(line, bytes_in_line, &is_valid);
    if(error) return error;
    if(is_valid) {
        *comment_type = COMMENT_TYPE_RESET;
        return S_OK;
    }

    return S_OK;
}

Err
_parse_legacy_file_comment_comment_reply_core_one_line_comment_reply(char *line, int bytes_in_line, LegacyFileInfo *legacy_file_info, int *comment_idx, enum LegacyFileStatus *status)
{
    Err error_code = S_OK;
    bool is_comment_line = false;
    error_code = _is_comment_line(line, bytes_in_line, &is_comment_line);
    if(error_code) return error_code;

    if(is_comment_line) {
        (*comment_idx)++;
        *status = LEGACY_FILE_STATUS_COMMENT;
        return S_OK;
    }

    legacy_file_info->comment_info[*comment_idx].comment_reply_len += bytes_in_line;

    return S_OK;
}

Err
_parse_legacy_file_comment_comment_reply_core_last_line(int bytes_in_line, char *line, LegacyFileInfo *legacy_file_info, int *comment_idx, time64_t *current_create_milli_timestamp, enum LegacyFileStatus *status)
{
    return _parse_legacy_file_comment_comment_reply_core_one_line(line, bytes_in_line, legacy_file_info, comment_idx, current_create_milli_timestamp, status);
}

/*****
 * Is Comment line
 *****/

Err
_is_comment_line(char *line, int bytes_in_line, bool *is_valid)
{
    Err error_code = S_OK;
    error_code = _is_comment_line_good_bad_arrow(line, bytes_in_line, is_valid, COMMENT_TYPE_GOOD);
    if(error_code) return error_code;
    if(*is_valid) return S_OK;

    error_code = _is_comment_line_good_bad_arrow(line, bytes_in_line, is_valid, COMMENT_TYPE_BAD);
    if(error_code) return error_code;
    if(*is_valid) return S_OK;

    error_code = _is_comment_line_good_bad_arrow(line, bytes_in_line, is_valid, COMMENT_TYPE_ARROW);
    if(error_code) return error_code;
    if(*is_valid) return S_OK;

    error_code = _is_comment_line_cross(line, bytes_in_line, is_valid);
    if(error_code) return error_code;
    if(*is_valid) return S_OK;

    error_code = _is_comment_line_reset(line, bytes_in_line, is_valid);
    if(error_code) return error_code;
    if(*is_valid) return S_OK;

    return S_OK;
}


Err
_is_comment_line_good_bad_arrow(char *line, int bytes_in_line, bool *is_valid, enum CommentType comment_type)
{
    if(bytes_in_line < LEN_COMMENT_HEADER) {
        *is_valid = false;
        return S_OK;
    }

    static char comment_header[COMMENT_BUF_SIZE] = {};
    sprintf(comment_header, "%s%s " ANSI_COLOR(33), COMMENT_TYPE_ATTR2[comment_type], COMMENT_TYPE_ATTR[comment_type]);

    //fprintf(stderr, "migrate_file_to_db._is_comment_good_bad_arrow: line: %s %u:%u:%u:%u:%u:%u:%u:%u:%u:%u comment_header: %s %d:%d:%d:%d:%d:%d:%d:%d:%d:%d COMMENT_TYPE_ATTR2: %d:%d:%d:%d:%d:%d:%d:%d:%d:%d\n", line, line[0], line[1], line[2], line[3], line[4], line[5], line[6], line[7], line[8], line[9], comment_header, comment_header[0], comment_header[1], comment_header[2], comment_header[3], comment_header[4], comment_header[5], comment_header[6], comment_header[7], comment_header[8], comment_header[9], COMMENT_TYPE_ATTR2[comment_type][0], COMMENT_TYPE_ATTR2[comment_type][1], COMMENT_TYPE_ATTR2[comment_type][2], COMMENT_TYPE_ATTR2[comment_type][3], COMMENT_TYPE_ATTR2[comment_type][4], COMMENT_TYPE_ATTR2[comment_type][5], COMMENT_TYPE_ATTR2[comment_type][6], COMMENT_TYPE_ATTR2[comment_type][7], COMMENT_TYPE_ATTR2[comment_type][8], COMMENT_TYPE_ATTR2[comment_type][9]);
    if(!strncmp(line, comment_header, LEN_COMMENT_HEADER)) {
        *is_valid = true;
    }
    else {
        *is_valid = false;
    }

    return S_OK;
}

Err
_is_comment_line_cross(char *line, int bytes_in_line, bool *is_valid)
{
    if(strncmp(line, COMMENT_TYPE_ATTR[COMMENT_TYPE_CROSS], 2)) {
        *is_valid = false;
        return S_OK;
    }

    char *p_line = line + 3;
    while(*p_line && *p_line != '\r' && *p_line != ':') p_line++;
    bytes_in_line -= p_line - line;

    if(*p_line != ':') {
        *is_valid = false;
        return S_OK;
    }

    if(bytes_in_line < LEN_COMMENT_CROSS_PREFIX) {
        *is_valid = false;
        return S_OK;
    }

    //fprintf(stderr, "migrate_file_to_db._is_comment_line_cross: p_line: %s\n", p_line);
    if(strncmp(p_line, COMMENT_CROSS_PREFIX, LEN_COMMENT_CROSS_PREFIX)) {
        *is_valid = false;
        return S_OK;
    }

    *is_valid = true;
    return S_OK;
}

Err
_is_comment_line_reset(char *line, int bytes_in_line, bool *is_valid)
{
    if(strncmp(line, COMMENT_TYPE_ATTR[COMMENT_TYPE_CROSS], 2)) {
        *is_valid = false;
        return S_OK;
    }

    char *p_line = line = 2;
    while(*p_line && *p_line != '\r' && *p_line != '\n' && *p_line != ' ') p_line++;
    bytes_in_line -= p_line - line;

    if(*p_line != ' ') {
        *is_valid = false;
        return S_OK;
    }

    // infix
    if(bytes_in_line < LEN_COMMENT_RESET_INFIX) {
        *is_valid = false;
        return S_OK;
    }

    if(strncmp(p_line, COMMENT_RESET_INFIX, LEN_COMMENT_RESET_INFIX)) {
        *is_valid = false;
        return S_OK;
    }

    p_line += LEN_COMMENT_RESET_INFIX;

    // mm
    int mm = atoi(p_line);
    if(mm < 1 || mm > 12) {
        *is_valid = false;
        return S_OK;
    }
    p_line += 2;

    if(*p_line != '/') {
        *is_valid = false;
        return S_OK;
    }
    p_line++;

    // dd
    int dd = atoi(p_line);
    if(dd < 1 || dd > 31) {
        *is_valid = false;
        return S_OK;
    }
    p_line += 2;

    if(*p_line != '/') {
        *is_valid = false;
        return S_OK;
    }
    p_line++;

    // yyyy
    int yyyy = atoi(p_line);
    if(yyyy < 1990) {
        *is_valid = false;
        return S_OK;
    }
    p_line += 4;

    if(*p_line != ' ') {
        *is_valid = false;
        return S_OK;
    }

    // HH
    int HH = atoi(p_line);
    if(HH < 0 || HH > 23) {
        *is_valid = false;
        return S_OK;
    }
    p_line += 2;

    if(*p_line != ':') {
        *is_valid = false;
        return S_OK;
    }
    p_line++;

    // MM
    int MM = atoi(p_line);
    if(MM < 0 || MM > 59) {
        *is_valid = false;
        return S_OK;
    }
    p_line += 2;

    if(*p_line != ':') {
        *is_valid = false;
        return S_OK;
    }
    p_line++;

    // SS
    int SS = atoi(p_line);
    if(SS < 0 || SS > 59) {
        *is_valid = false;
        return S_OK;
    }
    p_line += 2;

    if(*p_line != ' ') {
        *is_valid = false;
        return S_OK;
    }

    // postfix
    if(strncmp(p_line, COMMENT_RESET_POSTFIX, LEN_COMMENT_RESET_POSTFIX)) {
        *is_valid = false;
        return S_OK;
    }
}

Err
init_legacy_file_info_comment_comment_reply(LegacyFileInfo *legacy_file_info, int n_comment_comment_reply)
{
    if(legacy_file_info->comment_info) return S_ERR;

    legacy_file_info->comment_info = malloc(sizeof(LegacyCommentInfo) * n_comment_comment_reply);
    if(!legacy_file_info->comment_info) return S_ERR;

    legacy_file_info->n_comment_comment_reply = n_comment_comment_reply;

    return S_OK;
}

Err
safe_destroy_legacy_file_info(LegacyFileInfo *legacy_file_info)
{
    safe_free((void **)&legacy_file_info->comment_info);
    legacy_file_info->n_comment_comment_reply = 0;

    return S_OK;
}