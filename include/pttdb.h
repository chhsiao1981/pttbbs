/* $Id$ */
#ifndef PTTDB_H
#define PTTDB_H

#define UUIDLEN 64
#define _UUIDLEN 48

#define MAX_ORIGIN_LEN 20
#define MAX_WEB_LINK 50
#define MAX_BUF_SIZE 8192
#define MAX_BUF_BLOCK 8192
#define MAX_BUF_COMMENT 256
#define MAX_BUF_LINES 256

#define N_GEN_UUID_WITH_DB 10

// Mongo Post
#define MONGO_POST_DBNAME "post"

#define MONGO_MAIN_NAME "main"
#define MONGO_MAIN_CONTENT_NAME "main_content"

// Mongo Test
#define MONGO_TEST_DBNAME "test"

#define MONGO_TEST_NAME "test"

// XXX hack for time64_t and UUID
typedef long long int time64_t;
typedef unsigned char UUID[UUIDLEN];
typedef unsigned char _UUID[_UUIDLEN];
typedef unsigned char _UUID_b64[_UUIDB64LEN];

enum {
    MONGO_MAIN,
    MONGO_MAIN_CONTENT,

    MONGO_TEST,

    N_MONGO_COLLECTIONS,
};


enum {
    COMMENTTYPE_GOOD,
    COMMENTTYPE_BAD,
    COMMENTTYPE_ARROW,
    COMMENTTYPE_SIZE,

    COMMENTTYPE_FORWARD,                       // hack for forward
    COMMENTTYPE_OTHER,                         // hack for other

    COMMENTTYPE_MAX     = COMMENTTYPE_SIZE-1,
    COMMENTTYPE_DEFAULT = COMMENTTYPE_GOOD,
};

enum {
    KARMA_GOOD = 1,
    KARMA_BAD = -1,
    KARMA_ARROW = 0,
    KARMA_FORWARD = 0,
    KARMA_OTHER = 0,
};

enum {
    LIVE_STATUS_ALIVE,
    LIVE_STATUS_DELETED,
};

/**********
 * Main
 * XXX always update main first, and then update main-header.
 **********/
typedef struct MainHeader {
    unsigned char version;

    UUID the_id;
    UUID content_id;
    UUID update_content_id;
    aidu_t aid;

    unsigned char status;
    char status_updater[IDLEN + 1];
    unsigned char status_update_ip[IPV4LEN + 1];

    char title[TTLEN + 1];

    char poster[IDLEN + 1];
    unsigned char ip[IPV4LEN + 1];
    time64_t create_milli_timestamp;
    char updater[IDLEN + 1];
    unsigned char update_ip[IPV4LEN + 1];
    time64_t update_milli_timestamp;

    char origin[MAX_ORIGIN_LEN + 1];
    char web_link[MAX_WEB_LINK + 1];

    int reset_karma;

    int n_total_line;
    int n_total_block;
    int len_total;
} MainHeader;

typedef struct MainContent {
    UUID the_id;
    UUID main_id;

    int block_id;

    int len_block;
    int n_line;

    char buf_block[MAX_BUF_BLOCK + 1];
} MainContent;

/**********
 * Comments
 **********/
typedef struct Comment {
    unsigned char version;

    UUID the_id;
    UUID main_id;

    unsigned char status;    
    char status_updater[IDLEN + 1];
    unsigned char status_update_ip[IPV4LEN + 1];

    unsigned char rec_type;
    int karma;    

    char poster[IDLEN + 1];
    unsigned char ip[IPV4LEN + 1];
    time64_t create_milli_timestamp;
    char updater[IDLEN + 1];
    unsigned char update_ip[IPV4LEN + 1];
    time64_t update_milli_timestamp;

    int len;
    char buf[MAX_BUF_COMMENT + 1];
} Comment;

/**********
 * CommentReply
 * XXX always update comment-reply first, and then update comment-reply-header.
 **********/
typedef struct CommentReplyHeader {
    unsigned char version;

    UUID the_id;
    UUID content_id;
    UUID update_content_id;

    UUID comment_id;
    UUID main_id;

    unsigned char status;
    char status_updater[IDLEN + 1];
    unsigned char status_update_ip[IPV4LEN + 1];

    char poster[IDLEN + 1];
    unsigned char ip[IPV4LEN + 1];
    time64_t create_milli_timestamp;
    char updater[IDLEN + 1];
    unsigned char update_ip[IPV4LEN + 1];
    time64_t update_milli_timestamp;

    int n_total_line;
    int n_total_block;    
    int len_total;
} CommentReplyHeader;

typedef struct CommentReplyContent {
    UUID the_id;
    UUID comment_reply_id;

    int block_id;

    int len_block;
    int n_line;

    char buf_block[MAX_BUF_BLOCK + 1];
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
Err _serialize_uuid_bson(UUID uuid, bson_t *uuid_bson);
Err _serialize_content_uuid_bson(UUID uuid, int block_id, bson_t *uuid_bson);

/**********
 * Mongo
 **********/
Err init_mongo_global();
Err free_mongo_global();
Err init_mongo_collections();
Err free_mongo_collections();

Err db_set_if_not_exists(int collection, bson_t *key);
Err db_update_one(int collection, bson_t *key, bson_t *val, bool is_upsert);

Err _bson_get_value_int32(bson_t *b, char *name, int *value);

/**********
 * Post
 **********/
/*
Err n_line_post(UUID main_id, int *n_line);
*/

/**********
 * Main
 **********/
Err create_main_from_fd(aidu_t aid, char *title, char *poster, unsigned char *ip, unsigned char *origin, unsigned char *web_link, int len, int fd_content, UUID main_id);

/*
int len_main(UUID main_id);
int len_main_by_aid(aidu_t aid);

int n_line_main(UUID main_id);
int n_line_main_by_aid(aidu_t aid);

int read_main_header(UUID main_id, MainHeader *main_header);
int read_main_header_by_aid(aidu_t aid, MainHeader *main);
int read_main_contents(UUID main_content_id, int block_id, int max_n_main_content, int *n_read_main_content, MainContent *main_content);

int check_main(UUID main_id);

int update_main(UUID main_id, char *updater, unsigned char *ip, int len, char *content);
int update_main_by_aid(aidu_t aid, char *updater, unsigned char *ip, int len, char *content);

int delete_main(UUID main_id, char *updater, unsigned char *ip);
int delete_main_by_aid(aidu_t aid, char *updater, unsigned char *ip);
*/

Err _split_main_contents(int fd_content, int len, UUID main_id, UUID content_id, int *n_line, int *n_block);
Err _split_main_contents_core(char *line, int bytes_in_line, UUID main_id, UUID content_id, MainContent *main_content_block, int *n_line, int *n_block);
Err _split_main_contents_init_main_content(MainContent *main_content_block, UUID main_id, UUID content_id, int block_id);

Err _split_main_contents_save_main_content_block(MainContent *main_content_block);

Err _serialize_main_bson(MainHeader *main_header, bson_t *main_bson);

Err _serialize_main_content_block_bson(MainContent *main_content_block, bson_t *main_content_block_bson);

/**********
 * Comments
 **********/
/*
int create_comment(UUID main_id, char *poster, unsigned char *ip, int len, char *content, UUID *comment_id);

int count_karma_by_main(UUID main_id);
int len_comments_by_main(UUID main_id);
int n_line_comments_by_main(UUID main_id);
int read_comments_by_main(UUID main_id, time4_t create_timestamp, char *poster, int max_n_comments, int *n_read_comments, Comment *comments);
int read_comments_by_main_aid(aidu_t aid, time4_t create_timestamp, char *poster, int max_n_comments, int *n_read_comments, Comment *comments);

int update_comment(UUID comment_id, char *updater, unsigned char *ip, int len, char *content);

int delete_comment(UUID the_id, char *updater, unsigned char *ip);
*/

/**********
 * CommentReply
 **********/
/*
int create_comment_reply(UUID main_id, UUID comment_id, char *poster, unsigned char *ip, int len, char *content, UUID *comment_reply_id);

int len_comment_reply_by_main(UUID main_id);

int n_line_comment_reply_by_main(UUID main_id);
int n_line_comment_reply_by_comment(UUID comment_id);
int n_line_comment_reply_by_comment_reply(UUID comment_reply_id);

int read_comment_reply_header_by_comment(UUID comment_id, CommentReplyHeader *comment_reply_header);
int read_comment_reply_header_by_comment_reply_id(UUID comment_reply_id, CommentReplyHeader *comment_reply_header);
int read_comment_reply_contents(UUID comment_reply_content_id, int block_id, int max_n_comment_reply_content, int *n_read_comment_reply_content, CommentReplyContent * comment_reply_content);

int check_comment_reply(UUID comment_reply_id);

int update_comment_reply(UUID comment_reply_id, char *updater, unsigned char *ip, int len, char *content);

int delete_comment(UUID comment_reply_id, char *updater, unsigned char *ip);
*/

#endif /* PTTDB_H */