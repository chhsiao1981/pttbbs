#include "gtest/gtest.h"
#include "bbs.h"
#include "ptterr.h"
#include "pttdb.h"
#include "pttdb_internal.h"
#include "util_db_internal.h"

TEST(pttdb, save_content_block) {
    Err error;

    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);

    UUID ref_id;
    UUID content_id;
    gen_uuid(ref_id);
    gen_uuid(content_id);

    ContentBlock content_block = {};
    ContentBlock content_block2 = {};

    // init content-block
    error = reset_content_block(&content_block, ref_id, content_id, 3);
    EXPECT_EQ(S_OK, error);

    char buf[] = "test_buf\r\ntest2";
    int len_buf = strlen(buf);
    associate_content_block(&content_block, buf, len_buf);
    content_block.len_block = len_buf;
    content_block.n_line = 1;

    // init content-block2
    error = init_content_block_buf_block(&content_block2);
    EXPECT_EQ(S_OK, error);

    // save
    error = save_content_block(&content_block, MONGO_MAIN_CONTENT);
    EXPECT_EQ(S_OK, error);

    error = read_content_block(content_id, 3, MONGO_MAIN_CONTENT, &content_block2);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, strncmp(content_block2.buf_block, buf, len_buf));
    EXPECT_EQ(MAX_BUF_SIZE, content_block2.max_buf_len);
    EXPECT_EQ(len_buf, content_block2.len_block);
    EXPECT_EQ(1, content_block2.n_line);

    dissociate_content_block(&content_block);
    destroy_content_block(&content_block2);
}

TEST(pttdb, read_content_block_forgot_init) {
    Err error;

    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);

    UUID ref_id;
    UUID content_id;
    gen_uuid(ref_id);
    gen_uuid(content_id);

    ContentBlock content_block = {};
    ContentBlock content_block2 = {};

    // init content-block
    error = reset_content_block(&content_block, ref_id, content_id, 3);
    EXPECT_EQ(S_OK, error);

    char buf[] = "test_buf\r\ntest2";
    int len_buf = strlen(buf);
    associate_content_block(&content_block, buf, len_buf);
    content_block.len_block = len_buf;
    content_block.n_line = 1;

    // init content-block2
    // error = init_content_block_buf_block(&content_block2);
    // EXPECT_EQ(S_OK, error);

    // save
    error = save_content_block(&content_block, MONGO_MAIN_CONTENT);
    EXPECT_EQ(S_OK, error);

    error = read_content_block(content_id, 3, MONGO_MAIN_CONTENT, &content_block2);
    EXPECT_EQ(S_ERR, error);

    dissociate_content_block(&content_block);
    //destroy_content_block(&content_block2);
}

TEST(pttdb, reset_content_block) {
    Err error;
    ContentBlock content_block = {};

    UUID ref_id;
    UUID content_id;
    gen_uuid(ref_id);
    gen_uuid(content_id);

    error = reset_content_block(&content_block, ref_id, content_id, 3);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(NULL, content_block.buf_block);
    EXPECT_EQ(0, content_block.max_buf_len);
    EXPECT_EQ(0, strncmp((char *)ref_id, (char *)content_block.ref_id, UUIDLEN));
    EXPECT_EQ(0, strncmp((char *)content_id, (char *)content_block.the_id, UUIDLEN));
    EXPECT_EQ(3, content_block.block_id);

    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(NULL, content_block.buf_block);
    EXPECT_EQ(0, content_block.max_buf_len);
}

TEST(pttdb, init_content_block_with_buf_block) {
    Err error;
    ContentBlock content_block = {};

    UUID ref_id;
    UUID content_id;
    gen_uuid(ref_id);
    gen_uuid(content_id);

    error = init_content_block_with_buf_block(&content_block, ref_id, content_id, 3);
    EXPECT_EQ(S_OK, error);
    EXPECT_NE(NULL, (unsigned long)content_block.buf_block);
    EXPECT_EQ(MAX_BUF_SIZE, content_block.max_buf_len);
    EXPECT_EQ(0, strncmp((char *)ref_id, (char *)content_block.ref_id, UUIDLEN));
    EXPECT_EQ(0, strncmp((char *)content_id, (char *)content_block.the_id, UUIDLEN));
    EXPECT_EQ(3, content_block.block_id);

    error = destroy_content_block(&content_block);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(NULL, content_block.buf_block);
    EXPECT_EQ(0, content_block.max_buf_len);
}

TEST(pttdb, associate_content_block) {
    Err error;
    ContentBlock content_block = {};

    UUID ref_id;
    UUID content_id;
    gen_uuid(ref_id);
    gen_uuid(content_id);

    error = reset_content_block(&content_block, ref_id, content_id, 3);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(NULL, content_block.buf_block);
    EXPECT_EQ(0, content_block.max_buf_len);
    EXPECT_EQ(0, strncmp((char *)ref_id, (char *)content_block.ref_id, UUIDLEN));
    EXPECT_EQ(0, strncmp((char *)content_id, (char *)content_block.the_id, UUIDLEN));
    EXPECT_EQ(3, content_block.block_id);

    char buf[20];
    error = associate_content_block(&content_block, buf, 20);
    EXPECT_EQ(buf, content_block.buf_block);
    EXPECT_EQ(20, content_block.max_buf_len);

    error = dissociate_content_block(&content_block);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(NULL, content_block.buf_block);
    EXPECT_EQ(0, content_block.max_buf_len);
}

TEST(pttdb, serialize_content_block_bson) {
    ContentBlock content_block = {};
    ContentBlock content_block2 = {};

    init_content_block_buf_block(&content_block);
    init_content_block_buf_block(&content_block2);

    // initialize
    gen_uuid(content_block.the_id);
    gen_uuid(content_block.ref_id);
    content_block.block_id = 53;
    content_block.n_line = 1;
    char str[] = "test123\r\n";
    content_block.len_block = strlen(str);
    memcpy(content_block.buf_block, str, strlen(str));

    // init-op
    bson_t *b = NULL;
    
    // do-op
    Err error = _serialize_content_block_bson(&content_block, &b);
    EXPECT_EQ(S_OK, error);

    error = _deserialize_content_block_bson(b, &content_block2);
    EXPECT_EQ(S_OK, error);

    // post-op
    bson_safe_destroy(&b);

    // expect
    EXPECT_EQ(0, strncmp((char *)content_block.the_id, (char *)content_block2.the_id, UUIDLEN));
    EXPECT_EQ(0, strncmp((char *)content_block.ref_id, (char *)content_block2.ref_id, UUIDLEN));
    EXPECT_EQ(content_block.block_id, content_block2.block_id);
    EXPECT_EQ(content_block.len_block, content_block2.len_block);
    EXPECT_EQ(content_block.n_line, content_block2.n_line);
    EXPECT_STREQ(content_block.buf_block, content_block2.buf_block);

    destroy_content_block(&content_block);
    destroy_content_block(&content_block2);
}

TEST(pttdb, form_b_array_block_ids)
{
    bson_t *b = bson_new();

    Err error = _form_b_array_block_ids(5, 10, b);

    char *str = bson_as_canonical_extended_json(b, NULL);
    fprintf(stderr, "test_pttdb_content_block.form_b_array_block_ids: str: %s\n", str);
    free(str);

    bson_safe_destroy(b);
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

    const char *db_name[] = {
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
