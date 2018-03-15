#include "gtest/gtest.h"
#include "bbs.h"
#include "ptterr.h"
#include "pttdb.h"
#include "pttdb_internal.h"
#include "util_db_internal.h"

TEST(pttdb, create_comment) {
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);

    UUID main_id;
    char poster[IDLEN + 1] = {};
    char ip[IPV4LEN + 1] = {};
    char content[] = "temp_content";
    int len = strlen(content);
    enum CommentType comment_type = COMMENT_TYPE_GOOD;

    UUID comment_id;
    UUID tmp_comment_id;

    gen_uuid(main_id);
    Err error_code = create_comment(main_id, poster, ip, len, content, comment_type, comment_id);
    EXPECT_EQ(S_OK, error_code);

    Comment comment = {};
    init_comment_buf(&comment);

    memcpy(tmp_comment_id, comment_id, UUIDLEN);

    error_code = read_comment(comment_id, &comment);
    EXPECT_EQ(S_OK, error_code);

    EXPECT_EQ(0, strncmp((char *)tmp_comment_id, (char *)comment.the_id, UUIDLEN));

    EXPECT_EQ(0, strncmp((char *)main_id, (char *)comment.main_id, UUIDLEN));
    EXPECT_EQ(LIVE_STATUS_ALIVE, comment.status);

    EXPECT_STREQ(poster, comment.status_updater);
    EXPECT_STREQ(ip, comment.status_update_ip);

    EXPECT_EQ(comment_type, comment.comment_type);
    EXPECT_EQ(KARMA_GOOD, comment.karma);

    EXPECT_STREQ(poster, comment.poster);
    EXPECT_STREQ(ip, comment.ip);

    EXPECT_STREQ(poster, comment.updater);
    EXPECT_STREQ(ip, comment.update_ip);

    EXPECT_EQ(comment.create_milli_timestamp, comment.update_milli_timestamp);

    EXPECT_EQ(len, comment.len);
    EXPECT_STREQ(content, comment.buf);

    destroy_comment(&comment);
}

TEST(pttdb, delete_comment) {
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);

    UUID main_id;
    char poster[IDLEN + 1] = {};
    char ip[IPV4LEN + 1] = {};
    char content[] = "temp_content";
    int len = strlen(content);
    enum CommentType comment_type = COMMENT_TYPE_GOOD;

    UUID comment_id;

    gen_uuid(main_id);
    Err error = create_comment(main_id, poster, ip, len, content, comment_type, comment_id);
    EXPECT_EQ(S_OK, error);

    char del_updater[IDLEN + 1] = "del_updater";
    char status_update_ip[IPV4LEN + 1] = "10.1.1.4";
    error = delete_comment(comment_id, del_updater, status_update_ip);
    EXPECT_EQ(S_OK, error);

    char **fields;
    int n_fields = 3;
    fields = (char **)malloc(sizeof(char *) * n_fields);
    for (int i = 0; i < 3; i++) {
        fields[i] = (char *)malloc(30);
    }
    strcpy(fields[0], "status");
    strcpy(fields[1], "status_updater");
    strcpy(fields[2], "status_update_ip");

    bson_t *query = BCON_NEW(
                        "the_id", BCON_BINARY(comment_id, UUIDLEN)
                    );
    bson_t *result = NULL;

    error = db_find_one_with_fields(MONGO_COMMENT, query, fields, n_fields, &result);
    EXPECT_EQ(S_OK, error);
    int result_status;
    char result_status_updater[MAX_BUF_SIZE];
    char result_status_update_ip[MAX_BUF_SIZE];
    bson_get_value_int32(result, (char *)"status", &result_status);
    bson_get_value_bin(result, (char *)"status_updater", MAX_BUF_SIZE, result_status_updater, &len);
    bson_get_value_bin(result, (char *)"status_update_ip", MAX_BUF_SIZE, result_status_update_ip, &len);

    for (int i = 0; i < 3; i++) {
        free(fields[i]);
    }
    free(fields);

    EXPECT_EQ(LIVE_STATUS_DELETED, result_status);
    EXPECT_STREQ(del_updater, result_status_updater);
    EXPECT_STREQ(status_update_ip, result_status_update_ip);

    bson_safe_destroy(&query);
    bson_safe_destroy(&result);
}


TEST(pttdb_comment, serialize_comment_bson) {
    Comment comment = {};
    Comment comment2 = {};

    init_comment_buf(&comment);
    init_comment_buf(&comment2);

    comment.version = 2;
    gen_uuid(comment.the_id);
    gen_uuid(comment.main_id);
    comment.status = LIVE_STATUS_ALIVE;

    strcpy(comment.status_updater, "updater1");
    strcpy(comment.status_update_ip, "10.3.1.4");

    comment.comment_type = COMMENT_TYPE_GOOD;
    comment.karma = KARMA_GOOD;

    strcpy(comment.poster, "poster1");
    strcpy(comment.ip, "10.3.1.5");
    comment.create_milli_timestamp = 1514764800000; //2018-01-01 08:00:00 CST
    strcpy(comment.updater, "updater2");
    strcpy(comment.update_ip, "10.3.1.6");
    comment.update_milli_timestamp = 1514764801000; //2018-01-01 08:00:01 CST

    strcpy(comment.buf, "test_buf");
    comment.len = strlen(comment.buf);

    bson_t *comment_bson = NULL;

    Err error = _serialize_comment_bson(&comment, &comment_bson);
    EXPECT_EQ(S_OK, error);

    char *str = bson_as_canonical_extended_json(comment_bson, NULL);
    fprintf(stderr, "comment_bson: %s\n", str);
    bson_free(str);

    error = _deserialize_comment_bson(comment_bson, &comment2);

    bson_safe_destroy(&comment_bson);

    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(comment.version, comment2.version);
    EXPECT_EQ(0, strncmp((char *)comment.the_id, (char *)comment2.the_id, UUIDLEN));
    EXPECT_EQ(0, strncmp((char *)comment.main_id, (char *)comment2.main_id, UUIDLEN));
    EXPECT_EQ(comment.status, comment2.status);
    EXPECT_STREQ(comment.status_updater, comment2.status_updater);
    EXPECT_STREQ(comment.status_update_ip, comment2.status_update_ip);
    EXPECT_EQ(comment.comment_type, comment2.comment_type);
    EXPECT_EQ(comment.karma, comment2.karma);
    EXPECT_STREQ(comment.poster, comment2.poster);
    EXPECT_STREQ(comment.ip, comment2.ip);
    EXPECT_EQ(comment.create_milli_timestamp, comment2.create_milli_timestamp);
    EXPECT_STREQ(comment.updater, comment2.updater);
    EXPECT_STREQ(comment.update_ip, comment2.update_ip);
    EXPECT_EQ(comment.update_milli_timestamp, comment2.update_milli_timestamp);
    EXPECT_EQ(comment.len, comment2.len);
    EXPECT_STREQ(comment.buf, comment2.buf);

    destroy_comment(&comment);
    destroy_comment(&comment2);
}

TEST(pttdb_comment, get_comment_info_by_main) {

    UUID main_id;
    UUID comment_id;
    UUID comment_id2;

    gen_uuid(main_id);

    Err error = S_OK;
    error = create_comment(main_id, (char *)"poster1", (char *)"10.3.1.4", 10, (char *)"test1test1", COMMENT_TYPE_GOOD, comment_id);
    EXPECT_EQ(S_OK, error);
    error = create_comment(main_id, (char *)"poster1", (char *)"10.3.1.4", 10, (char *)"test2test2", COMMENT_TYPE_GOOD, comment_id2);
    EXPECT_EQ(S_OK, error);

    int n_total_comments = 0;
    int total_len = 0;
    error = get_comment_info_by_main(main_id, &n_total_comments, &total_len);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(2, n_total_comments);
    EXPECT_EQ(20, total_len);
}

TEST(pttdb_comment, get_comment_count_by_main) {

    UUID main_id;
    UUID comment_id;
    UUID comment_id2;

    gen_uuid(main_id);

    Err error = S_OK;
    error = create_comment(main_id, (char *)"poster1", (char *)"10.3.1.4", 10, (char *)"test1test1", COMMENT_TYPE_GOOD, comment_id);
    EXPECT_EQ(S_OK, error);
    error = create_comment(main_id, (char *)"poster1", (char *)"10.3.1.4", 10, (char *)"test2test2", COMMENT_TYPE_GOOD, comment_id2);
    EXPECT_EQ(S_OK, error);

    int n_total_comments = 0;
    error = get_comment_count_by_main(main_id, &n_total_comments);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(2, n_total_comments);
}

TEST(pttdb_comment, ensure_db_results_order) {
    int n_results = 100;
    bson_t **db_results = (bson_t **)malloc(sizeof(bson_t *) * n_results);
    long int rand_int = 0;

    for(int i = 0; i < n_results; i++) {
        rand_int = random();
        db_results[i] = BCON_NEW(
                "poster", BCON_BINARY((unsigned char *)"test_poster", 11),
                "create_milli_timestamp", BCON_INT64(rand_int)
            );    
    }

    Err error = _ensure_db_results_order(db_results, n_results, true);
    EXPECT_EQ(S_ERR, error);

    error = _ensure_db_results_order(db_results, n_results, false);
    EXPECT_EQ(S_ERR, error);

    for(int i = 0; i < n_results; i++) {
        bson_safe_destroy(&db_results[i]);
    }
    free(db_results);
}

TEST(pttdb_comment, ensure_db_results_order2) {
    int n_results = 100;
    bson_t **db_results = (bson_t **)malloc(sizeof(bson_t *) * n_results);

    for(int i = 0; i < n_results; i++) {
        db_results[i] = BCON_NEW(
                "poster", BCON_BINARY((unsigned char *)"test_poster", 11),
                "create_milli_timestamp", BCON_INT64(i)
            );    
    }

    Err error = _ensure_db_results_order(db_results, n_results, true);
    EXPECT_EQ(S_OK, error);

    error = _ensure_db_results_order(db_results, n_results, false);
    EXPECT_EQ(S_ERR, error);

    for(int i = 0; i < n_results; i++) {
        bson_safe_destroy(&db_results[i]);
    }
    free(db_results);
}

TEST(pttdb_comment, ensure_db_results_order3) {
    int n_results = 100;
    bson_t **db_results = (bson_t **)malloc(sizeof(bson_t *) * n_results);

    for(int i = 0; i < n_results; i++) {
        db_results[i] = BCON_NEW(
                "poster", BCON_BINARY((unsigned char *)"test_poster", 11),
                "create_milli_timestamp", BCON_INT64(100 - i)
            );    
    }

    Err error = _ensure_db_results_order(db_results, n_results, false);
    EXPECT_EQ(S_OK, error);

    error = _ensure_db_results_order(db_results, n_results, true);
    EXPECT_EQ(S_ERR, error);

    for(int i = 0; i < n_results; i++) {
        bson_safe_destroy(&db_results[i]);
    }
    free(db_results);
}

TEST(pttdb_comment, ensure_db_results_order4) {
    int n_results = 100;
    bson_t **db_results = (bson_t **)malloc(sizeof(bson_t *) * n_results);

    char poster[IDLEN + 1] = {}
    for(int i = 0; i < n_results; i++) {
        sprintf(poster, "poster%03d", i);
        db_results[i] = BCON_NEW(
                "poster", BCON_BINARY((unsigned char *)poster, IDLEN),
                "create_milli_timestamp", BCON_INT64(100)
            );    
    }

    Err error = _ensure_db_results_order(db_results, n_results, true);
    EXPECT_EQ(S_OK, error);

    error = _ensure_db_results_order(db_results, n_results, false);
    EXPECT_EQ(S_ERR, error);

    for(int i = 0; i < n_results; i++) {
        bson_safe_destroy(&db_results[i]);
    }
    free(db_results);
}

TEST(pttdb_comment, ensure_db_results_order5) {
    int n_results = 100;
    bson_t **db_results = (bson_t **)malloc(sizeof(bson_t *) * n_results);

    char poster[IDLEN + 1] = {}
    for(int i = 0; i < n_results; i++) {
        sprintf(poster, "poster%03d", 100 - i);
        db_results[i] = BCON_NEW(
                "poster", BCON_BINARY((unsigned char *)poster, IDLEN),
                "create_milli_timestamp", BCON_INT64(100)
            );    
    }

    Err error = _ensure_db_results_order(db_results, n_results, false);
    EXPECT_EQ(S_OK, error);

    error = _ensure_db_results_order(db_results, n_results, true);
    EXPECT_EQ(S_ERR, error);

    for(int i = 0; i < n_results; i++) {
        bson_safe_destroy(&db_results[i]);
    }
    free(db_results);
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

    FD = open("log.test_pttdb_comment.err", O_WRONLY | O_CREAT | O_TRUNC, 0660);
    dup2(FD, 2);

    const char *db_name[] = {
        "test_post",
        "test",
    };

    err = init_mongo_global();
    if (err != S_OK) {
        fprintf(stderr, "[ERROR] UNABLE TO init mongo global\n");
        return;
    }
    err = init_mongo_collections(db_name);
    if (err != S_OK) {
        fprintf(stderr, "[ERROR] UNABLE TO init mongo collections\n");
        return;
    }
}

void MyEnvironment::TearDown() {
    free_mongo_collections();
    free_mongo_global();

    if (FD) {
        close(FD);
        FD = 0;
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new MyEnvironment);

    return RUN_ALL_TESTS();
}
