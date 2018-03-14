/* $Id$ */
#ifndef PTTDB_H
#define PTTDB_H

#include "ptterr.h"
#include "util_db.h"

#include "bbs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UUIDLEN 64
#define _UUIDLEN 48

#define MAX_ORIGIN_LEN 20
#define MAX_WEB_LINK_LEN 100                  // MAX_ORIGN_LEN + 8 + 12 + BOARDLEN + 1 + 23
#define MAX_BUF_SIZE 8192
#define MAX_BUF_BLOCK 8192
#define MAX_BUF_COMMENT 256
#define MAX_BUF_LINES 256

#define N_GEN_UUID_WITH_DB 10

// XXX hack for time64_t and UUID
typedef long long int time64_t;
typedef unsigned char UUID[UUIDLEN];
typedef unsigned char _UUID[_UUIDLEN];

enum CommentType {
    COMMENT_TYPE_GOOD,
    COMMENT_TYPE_BAD,
    COMMENT_TYPE_ARROW,
    COMMENT_TYPE_SIZE,

    COMMENT_TYPE_FORWARD,                       // hack for forward
    COMMENT_TYPE_OTHER,                         // hack for other

    COMMENT_TYPE_N_TYPE,
    COMMENT_TYPE_MAX     = COMMENT_TYPE_SIZE - 1,
    COMMENT_TYPE_DEFAULT = COMMENT_TYPE_GOOD,
};

enum Karma {
    KARMA_GOOD = 1,
    KARMA_BAD = -1,
    KARMA_ARROW = 0,
};

enum LiveStatus {
    LIVE_STATUS_ALIVE,
    LIVE_STATUS_DELETED,
};

extern enum Karma KARMA_BY_COMMENT_TYPE[COMMENT_TYPE_N_TYPE];

/**********
 * Main
 * XXX always update main-content first, and then update main-header.
 **********/
typedef struct MainHeader {
    unsigned int version;                           // version

    UUID the_id;                                     // main id
    UUID content_id;                                 // corresponding content-id
    UUID update_content_id;                          // updating content-id, not effective if content_id == update_content_id
    aidu_t aid;                                      // aid

    enum LiveStatus status;                               // status of the main.
    char status_updater[IDLEN + 1];                  // last user updating the status
    char status_update_ip[IPV4LEN + 1];              // last ip updating the status

    char title[TTLEN + 1];                           // title

    char poster[IDLEN + 1];                          // creater
    char ip[IPV4LEN + 1];                            // create-ip
    time64_t create_milli_timestamp;                 // create-time
    char updater[IDLEN + 1];                         // last updater
    char update_ip[IPV4LEN + 1];                     // last update-ip
    time64_t update_milli_timestamp;                 // last update-time

    char origin[MAX_ORIGIN_LEN + 1];                 // origin
    char web_link[MAX_WEB_LINK_LEN + 1];                 // web-link

    int reset_karma;                                 // reset-karma.

    int n_total_line;                                // total-line
    int n_total_block;                               // total-block
    int len_total;                                   // total-size
} MainHeader;

typedef struct MainContent {
    UUID the_id;                                     // content-id
    UUID main_id;                                    // corresponding main-id

    int block_id;                                    // corresponding block-id

    int len_block;                                   // size of this block.
    int n_line;                                      // n-line of this block.

    char buf_block[MAX_BUF_BLOCK + 1];               // buf
} MainContent;

/**********
 * ContentBlock
 **********/
typedef struct ContentBlock {
    UUID the_id;                                     // content-id
    UUID ref_id;                                     // corresponding ref-id

    int block_id;                                    // corresponding block-id

    int len_block;                                   // size of this block.
    int n_line;                                      // n-line of this block.

    int max_buf_len;                                 // max buf len (not in db)
    char *buf_block;                                 // buf
} ContentBlock;

/**********
 * Comments
 **********/
typedef struct Comment {
    unsigned int version;                            // version

    UUID the_id;                                     // comment-id
    UUID main_id;                                    // corresponding main-id

    enum LiveStatus status;                               // status
    char status_updater[IDLEN + 1];                  // last user updaing the status
    char status_update_ip[IPV4LEN + 1];              // last ip updating the status

    enum CommentType comment_type;                        // comment-type.
    enum Karma karma;                                     // karma

    char poster[IDLEN + 1];                          // creater
    char ip[IPV4LEN + 1];                            // create-ip
    time64_t create_milli_timestamp;                 // create-time
    char updater[IDLEN + 1];                         // last updater
    char update_ip[IPV4LEN + 1];                     // last update-ip
    time64_t update_milli_timestamp;                 // last update-time

    int len;                                         // size
    char buf[MAX_BUF_COMMENT + 1];                   // buf
} Comment;

/**********
 * CommentReply
 * XXX always update comment-reply-content first, and then update comment-reply-header.
 **********/
typedef struct CommentReplyHeader {
    unsigned int version;                           // version

    UUID the_id;                                     // comment-reply-id
    UUID content_id;                                 // corresponding content-id
    UUID update_content_id;                          // updating content-id

    UUID comment_id;                                 // corresponding comment-id
    UUID main_id;                                    // corresponding main-id

    unsigned int status;                            // status
    char status_updater[IDLEN + 1];                  // last user updating status
    char status_update_ip[IPV4LEN + 1];              // last ip updating the status

    char poster[IDLEN + 1];                          // creater
    char ip[IPV4LEN + 1];                            // create-ip
    time64_t create_milli_timestamp;                 // create-time
    char updater[IDLEN + 1];                         // last updater
    char update_ip[IPV4LEN + 1];                     // last update-ip
    time64_t update_milli_timestamp;                 // last update-time

    int n_total_line;                                // total-line
    int n_total_block;                               // total-blocks
    int len_total;                                   // total-size
} CommentReplyHeader;

typedef struct CommentReplyContent {
    UUID the_id;                                     // comment-reply-content-id
    UUID comment_reply_id;                           // corresponding comment-reply-id

    int block_id;                                    // block-id

    int len_block;                                   // size of the block
    int n_line;                                      // lines of the block
    char buf_block[MAX_BUF_BLOCK + 1];               // buf
} CommentReplyContent;


/**********
 * Milli-timestamp
 **********/
Err get_milli_timestamp(time64_t *milli_timestamp);

/**********
 * UUID
 **********/
Err gen_uuid(UUID uuid);
Err gen_uuid_with_db(int collection, UUID uuid);
Err gen_content_uuid_with_db(int collection, UUID uuid);

Err uuid_to_milli_timestamp(UUID uuid, time64_t *milli_timestamp);

/**********
 * Post
 **********/
/*
Err n_line_post(UUID main_id, int *n_line);
*/

/**********
 * Main
 **********/
/*
Err create_main_from_fd(aidu_t aid, char *title, char *poster, char *ip, char *origin, char *web_link, int len, int fd_content, UUID main_id, UUID content_id);

Err len_main(UUID main_id, int *len);
Err len_main_by_aid(aidu_t aid, int *len);

Err n_line_main(UUID main_id, int *n_line);
Err n_line_main_by_aid(aidu_t aid, int *n_line);

Err read_main_header(UUID main_id, MainHeader *main_header);
Err read_main_header_by_aid(aidu_t aid, MainHeader *main_header);

Err read_main_content(UUID main_content_id, int block_id, MainContent *main_content);

Err delete_main(UUID main_id, char *updater, char *ip);
Err delete_main_by_aid(aidu_t aid, char *updater, char *ip);

Err update_main_from_fd(UUID main_id, char *updater, char *update_ip, int len, int fd_content, UUID content_id);
*/

/**********
 * ContentBlock
 **********/
Err split_contents(char *buf, int bytes, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, int *n_line, int *n_block);
Err split_contents_from_fd(int fd_content, int len, UUID ref_id, UUID content_id, enum MongoDBId mongo_db_id, int *n_line, int *n_block);

Err save_content_block(ContentBlock *content_block, enum MongoDBId mongo_db_id);
Err read_content_block(UUID content_id, int block_id, enum MongoDBId mongo_db_id, ContentBlock *content_block);
Err read_content_blocks(UUID content_id, int max_n_block, int block_id, enum MongoDBId mongo_db_id, ContentBlock *content_blocks, int *n_block, int *len);
Err dynamic_read_content_blocks(UUID content_id, int max_n_block, int block_id, enum MongoDBId mongo_db_id, char *buf, int max_buf_size, ContentBlock *content_blocks, int *n_block, int *len);

Err delete_content(UUID content_id, enum MongoDBId mongo_db_id);

Err init_content_block_with_buf_block(ContentBlock *content_block, UUID ref_id, UUID content_id, int block_id);
Err init_content_block_buf_block(ContentBlock *content_block);
Err destroy_content_block(ContentBlock *content_block);

Err reset_content_block(ContentBlock *content_block, UUID ref_id, UUID content_id, int block_id);
Err reset_content_block_buf_block(ContentBlock *content_block);

Err associate_content_block(ContentBlock *content_block, char *buf_block, int max_buf_len);
Err dissociate_content_block(ContentBlock *content_block);

/**********
 * Comments
 **********/
/*
Err create_comment(UUID main_id, char *poster, char *ip, int len, char *content, enum CommentType comment_type, UUID comment_id);

Err read_comment(UUID comment_id, Comment *comment);

Err delete_comment(UUID comment_id, char *updater, char *ip);
*/

/*
Err count_karma_by_main(UUID main_id);
Err len_comments_by_main(UUID main_id);
Err n_line_comments_by_main(UUID main_id);
Err read_comments_by_main(UUID main_id, time4_t create_timestamp, char *poster, int max_n_comments, int *n_read_comments, Comment *comments);
Err read_comments_by_main_aid(aidu_t aid, time4_t create_timestamp, char *poster, int max_n_comments, int *n_read_comments, Comment *comments);

Err update_comment(UUID comment_id, char *updater, unsigned char *ip, int len, char *content);

Err delete_comment(UUID the_id, char *updater, unsigned char *ip);
*/

/**********
 * CommentReply
 **********/
/*
Err create_comment_reply(UUID main_id, UUID comment_id, char *poster, unsigned char *ip, int len, char *content, UUID *comment_reply_id);

Err len_comment_reply_by_main(UUID main_id);

Err n_line_comment_reply_by_main(UUID main_id);
Err n_line_comment_reply_by_comment(UUID comment_id);
Err n_line_comment_reply_by_comment_reply(UUID comment_reply_id);

Err read_comment_reply_header_by_comment(UUID comment_id, CommentReplyHeader *comment_reply_header);
Err read_comment_reply_header_by_comment_reply_id(UUID comment_reply_id, CommentReplyHeader *comment_reply_header);
Err read_comment_reply_contents(UUID comment_reply_content_id, int block_id, int max_n_comment_reply_content, int *n_read_comment_reply_content, CommentReplyContent * comment_reply_content);

Err check_comment_reply(UUID comment_reply_id);

Err update_comment_reply(UUID comment_reply_id, char *updater, unsigned char *ip, int len, char *content);

Err delete_comment(UUID comment_reply_id, char *updater, unsigned char *ip);
*/

#ifdef __cplusplus
}
#endif

#endif /* PTTDB_H */