#include "bbs.h"

int len_post(UUID main_id, int *len) {
    int error_code = S_OK;
    int the_len_main = 0;
    int the_len_comments = 0;
    int the_len_comment_reply = 0;

    error_code = len_main(main_id, &the_len_main);
    if(error_code) return error_code;

    error_code = len_comments(main_id, &the_len_comments);
    if(error_code) return error_code;

    error_code = len_comment_reply_by_main(main_id, &the_len_comment_reply);
    if(error_code) return error_code;

    *len = the_len_main + the_len_comments + the_len_comment_reply;

    return S_OK;
}

int n_line_post(UUID main_id, int *n_line) {
    int error_code = S_OK;
    int the_line_main = 0;
    int the_line_comments = 0;
    int the_line_comment_reply = 0;

    error_code = n_line_main(main_id, &the_line_main);
    if(error_code) return error_code;

    error_code = n_line_comments(main_id, &the_line_comments);
    if(error_code) return error_code;

    error_code = n_line_comment_reply_by_main(main_id, &the_line_comment_reply);
    if(error_code) return error_code;

    *n_line = the_line_main + the_line_comments + the_line_comment_reply;

    return S_OK;
}

int create_main(aidu_t aid, char *title, char *poster, unsigned char *ip, unsigned char *origin, unsigned char *web_link, int len, char *content, UUID *main_id) {

    int error_code = S_OK;
    int n_lines;
    int n_main_contents;
    UUID main_id;
    UUID content_id;
    MainContent *main_contents;
    time8_t create_milli_timestamp;

    MainHeader main_header;

    error_code = _get_milli_timestamp(&create_milli_timestamp);
    if(error_code) return error_code;

    error_code = _get_content_lines(len, content, &n_lines);
    if(error_code) return error_code;

    error_code = gen_uuid_with_db('main', &main_id);
    if(error_code) return error_code;
    
    error_code = gen_uuid_with_db('main_content', &content_id);
    if(error_code) return error_code;

    n_main_contents = n_lines / MAX_BUF_LINES;
    if(n_main_contents * MAX_BUF_LINES != n_lines) n_main_contents++;

    // main_header
    main_header.the_id = main_id;
    main_header.content_id = content_id;
    main_header.aid = aid;
    main_header.status = LIVE_STATUS_ALIVE;
    strcpy(main_header.status_updater, poster);
    strcpy(main_header.status_update_ip, ip);
    strcpy(main_header.title, title);
    strcpy(main_header.poster, poster);
    strcpy(main_header.ip, ip);
    main_header.create_milli_timestamp = create_milli_timestamp;
    main_header.update_milli_timestamp = create_milli_timestamp;
    strcpy(main_header.origin, origin);
    strcpy(main_header.web_link, web_link);
    main_header.reset_karma = 0;

    main_header.n_total_line = n_lines;
    main_header.n_total_block = n_main_contents;
    main_header.len_total = len;

    main_contents = malloc(sizeof(MainContent) * n_main_contents);
    if(main_contents == NULL) return S_ERR_MALLOC;

    error_code = split_main_contents(len, content, n_main_contents, &main_contents);
    if(error_code) {
        free(main_contents);
        return error_code;
    }

    for(int i = 0; i < n_main_contents; i++) {
        main_contents[i].the_id = content_id;
        main_contents[i].main_id = main_id;
    }
}