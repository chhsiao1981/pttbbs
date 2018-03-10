#include "gtest/gtest.h"
#include "bbs.h"
#include "ptterr.h"
#include "pttdb.h"
#include "pttdb_internal.h"
#include "util_db_internal.h"

TEST(pttdb_comment, serialize_comment_bson) {
    Comment comment = {};
    Comment comment2 = {};

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

    bson_t comment_bson;
    bson_init(&comment_bson);

    Err error = _serialize_comment_bson(&comment, &comment_bson);
    EXPECT_EQ(S_OK, error);

    char *str = bson_as_canonical_extended_json(&comment_bson, NULL);
    fprintf(stderr, "comment_bson: %s\n", str);
    bson_free(str);

    error = _deserialize_comment_bson(&comment_bson, &comment2);

    bson_destroy(&comment_bson);

    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(comment.version, comment2.version);
    EXPECT_EQ(0, strncmp((char *)comment.the_id, (char *)comment2.the_id));
    EXPECT_EQ(0, strncmp((char *)comment.main_id, (char *)comment2.main_id));
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
