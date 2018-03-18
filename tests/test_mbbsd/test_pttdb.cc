#include "gtest/gtest.h"
#include "bbs.h"
#include "ptterr.h"
#include "pttdb.h"
#include "pttdb_internal.h"
#include "util_db_internal.h"

TEST(pttdb, n_line_post) {
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);

    int fd = open("data_test/test1.txt", O_RDONLY);

    aidu_t aid = 12345;
    char board[IDLEN + 1] = {};
    char title[TTLEN + 1] = {};
    char poster[IDLEN + 1] = {};
    char ip[IPV4LEN + 1] = {};
    char origin[MAX_ORIGIN_LEN + 1] = {};
    char web_link[MAX_WEB_LINK_LEN + 1] = {};
    int len = 10020;
    UUID main_id = {};
    UUID content_id = {};

    strcpy(board, "test_board");
    strcpy(title, "test_title");
    strcpy(poster, "test_poster");
    strcpy(ip, "test_ip");
    strcpy(origin, "ptt.cc");
    strcpy(web_link, "http://www.ptt.cc/bbs/alonglonglongboard/M.1234567890.ABCD.html");

    // create-main-from-fd
    Err error = create_main_from_fd(aid, board, title, poster, ip, origin, web_link, len, fd, main_id, content_id);
    EXPECT_EQ(S_OK, error);


    close(fd);

    // create-comment
    UUID comment_id = {};
    UUID comment_id2 = {};

    error = create_comment(main_id, (char *)"poster1", (char *)"10.3.1.4", 10, (char *)"test1test1", COMMENT_TYPE_GOOD, comment_id);
    EXPECT_EQ(S_OK, error);
    error = create_comment(main_id, (char *)"poster1", (char *)"10.3.1.4", 10, (char *)"test2test2", COMMENT_TYPE_GOOD, comment_id2);

    // comment-reply
    UUID comment_reply_id = {};
    UUID comment_reply_id2 = {};

    error = create_comment_reply(main_id, comment_id, (char *)"poster1", (char *)"10.3.1.4", 24, (char *)"test1test1\r\ntest3test3\r\n", comment_reply_id);
    EXPECT_EQ(S_OK, error);
    error = create_comment_reply(main_id, comment_id2, (char *)"poster1", (char *)"10.3.1.4", 12, (char *)"test2test2\r\n", comment_reply_id2);
    EXPECT_EQ(S_OK, error);

    int n_line;
    error = n_line_post(main_id, &n_line);
    EXPECT_EQ(S_OK, error)    ;
    EXPECT_EQ(15, n_line);
}

TEST(pttdb, get_file_info_by_main_get_main_info) {
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);


    int fd = open("data_test/test1.txt", O_RDONLY);

    aidu_t aid = 12345;
    char board[IDLEN + 1] = {};
    char title[TTLEN + 1] = {};
    char poster[IDLEN + 1] = {};
    char ip[IPV4LEN + 1] = {};
    char origin[MAX_ORIGIN_LEN + 1] = {};
    char web_link[MAX_WEB_LINK_LEN + 1] = {};
    int len = 10020;
    UUID main_id = {};
    UUID content_id = {};

    strcpy(board, "test_board");
    strcpy(title, "test_title");
    strcpy(poster, "test_poster");
    strcpy(ip, "test_ip");
    strcpy(origin, "ptt.cc");
    strcpy(web_link, "http://www.ptt.cc/bbs/alonglonglongboard/M.1234567890.ABCD.html");

    // create-main-from-fd
    Err error = create_main_from_fd(aid, board, title, poster, ip, origin, web_link, len, fd, main_id, content_id);
    EXPECT_EQ(S_OK, error);

    close(fd);

    FileInfo file_info = {};

    error = _get_file_info_by_main_get_main_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(0, strncmp((char *)main_id, (char *)file_info.main_id, UUIDLEN));
    EXPECT_STREQ(poster, file_info.main_updater);
    EXPECT_EQ(0, strncmp((char *)content_id, (char *)file_info.main_content_id, UUIDLEN));
    EXPECT_EQ(10, file_info.n_main_line);
    EXPECT_EQ(2, file_info.n_main_block);
    EXPECT_EQ(0, file_info.n_comment);
    EXPECT_EQ(NULL, file_info.content_block_info);
    EXPECT_EQ(NULL, file_info.comment_comment_reply_info);

    destroy_file_info(&file_info);
}

TEST(pttdb, get_file_info_by_main_get_content_block_info) {
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);


    int fd = open("data_test/test1.txt", O_RDONLY);

    aidu_t aid = 12345;
    char board[IDLEN + 1] = {};
    char title[TTLEN + 1] = {};
    char poster[IDLEN + 1] = {};
    char ip[IPV4LEN + 1] = {};
    char origin[MAX_ORIGIN_LEN + 1] = {};
    char web_link[MAX_WEB_LINK_LEN + 1] = {};
    int len = 10020;
    UUID main_id = {};
    UUID content_id = {};

    strcpy(board, "test_board");
    strcpy(title, "test_title");
    strcpy(poster, "test_poster");
    strcpy(ip, "test_ip");
    strcpy(origin, "ptt.cc");
    strcpy(web_link, "http://www.ptt.cc/bbs/alonglonglongboard/M.1234567890.ABCD.html");

    // create-main-from-fd
    Err error = create_main_from_fd(aid, board, title, poster, ip, origin, web_link, len, fd, main_id, content_id);
    EXPECT_EQ(S_OK, error);

    close(fd);

    FileInfo file_info = {};

    // get file info by main get main-info
    error = _get_file_info_by_main_get_main_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);

    // get file info by main get content block_info
    error = _get_file_info_by_main_get_content_block_info(&file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(2, file_info.n_main_block);
    EXPECT_EQ(0, file_info.n_comment);
    EXPECT_NE(NULL, (long)file_info.content_block_info);
    EXPECT_EQ(NULL, (long)file_info.comment_comment_reply_info);

    EXPECT_EQ(0, file_info.content_block_info[0].block_id);
    EXPECT_EQ(8, file_info.content_block_info[0].n_line);
    EXPECT_EQ(1, file_info.content_block_info[1].block_id);
    EXPECT_EQ(2, file_info.content_block_info[1].n_line);

    destroy_file_info(&file_info);
}

TEST(pttdb, get_file_info_by_main_get_comment_comment_reply_info_no_comments) {
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);


    int fd = open("data_test/test1.txt", O_RDONLY);

    aidu_t aid = 12345;
    char board[IDLEN + 1] = {};
    char title[TTLEN + 1] = {};
    char poster[IDLEN + 1] = {};
    char ip[IPV4LEN + 1] = {};
    char origin[MAX_ORIGIN_LEN + 1] = {};
    char web_link[MAX_WEB_LINK_LEN + 1] = {};
    int len = 10020;
    UUID main_id = {};
    UUID content_id = {};

    strcpy(board, "test_board");
    strcpy(title, "test_title");
    strcpy(poster, "test_poster");
    strcpy(ip, "test_ip");
    strcpy(origin, "ptt.cc");
    strcpy(web_link, "http://www.ptt.cc/bbs/alonglonglongboard/M.1234567890.ABCD.html");

    // create-main-from-fd
    Err error = create_main_from_fd(aid, board, title, poster, ip, origin, web_link, len, fd, main_id, content_id);
    EXPECT_EQ(S_OK, error);

    close(fd);

    FileInfo file_info = {};

    // get file info by main get main-info
    error = _get_file_info_by_main_get_main_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);

    // get file info by main get content block_info
    error = _get_file_info_by_main_get_content_block_info(&file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(2, file_info.n_main_block);
    EXPECT_EQ(0, file_info.n_comment);
    EXPECT_NE(NULL, (long)file_info.content_block_info);
    EXPECT_EQ(NULL, (long)file_info.comment_comment_reply_info);

    EXPECT_EQ(0, file_info.content_block_info[0].block_id);
    EXPECT_EQ(8, file_info.content_block_info[0].n_line);
    EXPECT_EQ(1, file_info.content_block_info[1].block_id);
    EXPECT_EQ(2, file_info.content_block_info[1].n_line);

    // get file info by main get comment comment reply info
    error = _get_file_info_by_main_get_comment_comment_reply_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(0, file_info.n_comment);
    EXPECT_EQ(NULL, (long)file_info.comment_comment_reply_info);

    destroy_file_info(&file_info);
}

TEST(pttdb, get_file_info_by_main_get_comment_comment_reply_info_comments_no_replys) {
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);


    int fd = open("data_test/test1.txt", O_RDONLY);

    aidu_t aid = 12345;
    char board[IDLEN + 1] = {};
    char title[TTLEN + 1] = {};
    char poster[IDLEN + 1] = {};
    char ip[IPV4LEN + 1] = {};
    char origin[MAX_ORIGIN_LEN + 1] = {};
    char web_link[MAX_WEB_LINK_LEN + 1] = {};
    int len = 10020;
    UUID main_id = {};
    UUID content_id = {};

    strcpy(board, "test_board");
    strcpy(title, "test_title");
    strcpy(poster, "test_poster");
    strcpy(ip, "test_ip");
    strcpy(origin, "ptt.cc");
    strcpy(web_link, "http://www.ptt.cc/bbs/alonglonglongboard/M.1234567890.ABCD.html");

    // create-main-from-fd
    Err error = create_main_from_fd(aid, board, title, poster, ip, origin, web_link, len, fd, main_id, content_id);
    EXPECT_EQ(S_OK, error);

    close(fd);

    // comment and comment-reply
    Comment comment = {};
    init_comment_buf(&comment);

    memcpy(comment.main_id, main_id, sizeof(UUID));
    comment.status = LIVE_STATUS_ALIVE;
    strcpy(comment.status_updater, "poster000");
    strcpy(comment.status_update_ip, "10.1.1.4");

    time64_t create_milli_timestamp = 1514764800000; //2018-01-01 08:00:00 CST

    comment.comment_type = COMMENT_TYPE_GOOD;
    comment.karma = KARMA_BY_COMMENT_TYPE[COMMENT_TYPE_GOOD];

    strcpy(comment.poster, "poster001");
    strcpy(comment.ip, "10.1.1.4");
    comment.create_milli_timestamp = create_milli_timestamp;
    strcpy(comment.updater, "poster000");
    strcpy(comment.update_ip, "10.1.1.4");
    comment.update_milli_timestamp = create_milli_timestamp;

    strcpy(comment.buf, "test1test1\r\n");
    comment.len = 10;

    bson_t *comment_id_bson = NULL;
    bson_t *comment_bson = NULL;

    char buf[MAX_BUF_SIZE] = {};
    char *p_buf = NULL;

    UUID comment_id = {};
    for(int i = 85; i < 100; i++) {
        gen_uuid(comment_id);
        memcpy(comment.the_id, comment_id, sizeof(UUID));
        sprintf(comment.poster, "poster%03d", i);

        comment.create_milli_timestamp = create_milli_timestamp + 85;
        comment.update_milli_timestamp = create_milli_timestamp + 85;

        error = _serialize_comment_bson(&comment, &comment_bson);
        error = _serialize_uuid_bson(comment_id, &comment_id_bson);
        
        error = db_update_one(MONGO_COMMENT, comment_id_bson, comment_bson, true);

        bson_safe_destroy(&comment_bson);
        bson_safe_destroy(&comment_id_bson);

        EXPECT_EQ(S_OK, error);

        p_buf = buf;
        for(int j = 0; j < i; j++) {
            sprintf(p_buf, "testtest\r\n");
            p_buf += 10;
        }

        //error = create_comment_reply(main_id, comment_id, (char *)"reply001", (char *)"10.1.1.5", i * 10, p_buf, comment_reply_id);
        //EXPECT_EQ(S_OK, error);
        //EXPECT_EQ(0, strncmp((char *)comment_id, (char *)comment_reply_id, UUIDLEN));
    }

    for(int i = 15; i < 85; i++) {
        gen_uuid(comment_id);
        memcpy(comment.the_id, comment_id, sizeof(UUID));
        sprintf(comment.poster, "poster%03d", i);

        comment.create_milli_timestamp = create_milli_timestamp + i;
        comment.update_milli_timestamp = create_milli_timestamp + i;

        error = _serialize_comment_bson(&comment, &comment_bson);
        error = _serialize_uuid_bson(comment_id, &comment_id_bson);
        
        error = db_update_one(MONGO_COMMENT, comment_id_bson, comment_bson, true);

        bson_safe_destroy(&comment_bson);
        bson_safe_destroy(&comment_id_bson);

        EXPECT_EQ(S_OK, error);

        p_buf = buf;
        for(int j = 0; j < i; j++) {
            sprintf(p_buf, "testtest\r\n");
            p_buf += 10;
        }

        //error = create_comment_reply(main_id, comment_id, (char *)"reply001", (char *)"10.1.1.5", i * 10, p_buf, comment_reply_id);
        //EXPECT_EQ(S_OK, error);
        //EXPECT_EQ(0, strncmp((char *)comment_id, (char *)comment_reply_id, UUIDLEN));
    }

    int the_i = 0;
    for(int i = 0; i < 15; i++) {
        gen_uuid(comment_id);
        memcpy(comment.the_id, comment_id, sizeof(UUID));
        sprintf(comment.poster, "poster%03d", i);

        comment.create_milli_timestamp = create_milli_timestamp;
        comment.update_milli_timestamp = create_milli_timestamp;

        error = _serialize_comment_bson(&comment, &comment_bson);
        error = _serialize_uuid_bson(comment_id, &comment_id_bson);

        error = db_update_one(MONGO_COMMENT, comment_id_bson, comment_bson, true);

        bson_safe_destroy(&comment_bson);
        bson_safe_destroy(&comment_id_bson);

        EXPECT_EQ(S_OK, error);

        the_i = i ? i : 1;

        p_buf = buf;
        for(int j = 0; j < the_i; j++) {
            sprintf(p_buf, "testtest\r\n");
            p_buf += 10;
        }

        //create_comment_reply(main_id, comment_id, (char *)"reply001", (char *)"10.1.1.5", i * 10, p_buf, comment_reply_id);
        //EXPECT_EQ(S_OK, error);
        //EXPECT_EQ(0, strncmp((char *)comment_id, (char *)comment_reply_id, UUIDLEN));
    }

    // get file info by main get main-info
    FileInfo file_info = {};

    error = _get_file_info_by_main_get_main_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);

    // get file info by main get content block_info
    error = _get_file_info_by_main_get_content_block_info(&file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(2, file_info.n_main_block);
    EXPECT_EQ(0, file_info.n_comment);
    EXPECT_NE(NULL, (long)file_info.content_block_info);
    EXPECT_EQ(NULL, (long)file_info.comment_comment_reply_info);

    EXPECT_EQ(0, file_info.content_block_info[0].block_id);
    EXPECT_EQ(8, file_info.content_block_info[0].n_line);
    EXPECT_EQ(1, file_info.content_block_info[1].block_id);
    EXPECT_EQ(2, file_info.content_block_info[1].n_line);

    // get file info by main get comment comment reply info
    error = _get_file_info_by_main_get_comment_comment_reply_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(100, file_info.n_comment);
    EXPECT_NE(NULL, (long)file_info.comment_comment_reply_info);

    char cmp_poster[IDLEN + 1] = {};
    for(int i = 0; i < 100; i++) {
        sprintf(cmp_poster, "poster%03d", i);
        EXPECT_STREQ(cmp_poster, file_info.comment_comment_reply_info[i].comment_poster);
    }

    // free
    destroy_file_info(&file_info);
}

/**********
 * MAIN
 */
int FD = 0;

class MyEnvironment: public ::testing::Environment {
public:
    void SetUp();
    void TearDown();
};

void MyEnvironment::SetUp() {
    Err err = S_OK;

    FD = open("log.test_pttdb.err", O_WRONLY|O_CREAT|O_TRUNC, 0660);
    dup2(FD, 2);

    const char *db_name[] = {
        "test_post",
        "test",
    };

    err = init_mongo_global();
    if(err != S_OK) {
        fprintf(stderr, "[ERROR] UNABLE TO init mongo global\n");
        return;
    }
    err = init_mongo_collections(db_name);
    if(err != S_OK) {
        fprintf(stderr, "[ERROR] UNABLE TO init mongo collections\n");
        return;
    }

    FILE *f = fopen("data_test/test1.txt", "w");
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < 1000; i++) {
            fprintf(f, "%c", 64 + (i % 26));
        }
        fprintf(f, "\r\n");
    }
    fclose(f);    
}

void MyEnvironment::TearDown() {
    free_mongo_collections();
    free_mongo_global();

    if(FD) {
        close(FD);
        FD = 0;
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new MyEnvironment);

    return RUN_ALL_TESTS();
}
