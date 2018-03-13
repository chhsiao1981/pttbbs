#include "gtest/gtest.h"
#include "bbs.h"
#include "ptterr.h"
#include "pttdb.h"
#include "pttdb_internal.h"
#include "util_db_internal.h"

TEST(pttdb, serialize_content_block_bson) {
    ContentBlock content_block = {};
    ContentBlock content_block2 = {};

    // initialize
    gen_uuid(content_block.the_id);
    gen_uuid(content_block.main_id);
    content_block.block_id = 53;    
    content_block.n_line = 2;
    const char str[] = "test123\r\n";
    content_block.len_block = strlen(str);
    memcpy(content_block.buf_block, str, strlen(str));

    // init-op
    bson_t b;
    bson_init(&b);
    
    // do-op
    Err error = _serialize_content_block_bson(&content_block, &b);
    EXPECT_EQ(S_OK, error);

    error = _deserialize_content_block_bson(&b, &content_block2);
    EXPECT_EQ(S_OK, error);

    // post-op
    bson_destroy(&b);

    // expect
    EXPECT_EQ(0, strncmp((char *)content_block.the_id, (char *)content_block2.the_id, UUIDLEN));
    EXPECT_EQ(0, strncmp((char *)content_block.ref_id, (char *)content_block2.ref_id, UUIDLEN));
    EXPECT_EQ(content_block.block_id, content_block2.block_id);
    EXPECT_EQ(content_block.len_block, content_block2.len_block);
    EXPECT_EQ(content_block.n_line, content_block2.n_line);
    EXPECT_STREQ(content_block.buf_block, content_block2.buf_block);
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

    FD = open("log.test_pttdb_content_block.err", O_WRONLY | O_CREAT | O_TRUNC, 0660);
    dup2(FD, 2);

    const char *DEFAULT_MONGO_DB[] = {
        "test_post",      //MONGO_POST_DBNAME
        "test",      //MONGO_TEST_DBNAME
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
