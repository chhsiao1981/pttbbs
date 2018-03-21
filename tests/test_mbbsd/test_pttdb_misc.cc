#include "gtest/gtest.h"
#include "bbs.h"
#include "ptterr.h"
#include "pttdb.h"
#include "pttdb_internal.h"
#include "util_db_internal.h"

TEST(pttdb_mics, get_line_from_buf) {
    int len_buf = 24;
    char buf[MAX_BUF_SIZE];
    char line[MAX_BUF_SIZE];
    int offset_buf = 0;
    int offset_line = 0;
    int bytes_in_new_line = 0;
    bzero(line, sizeof(line));

    strcpy(buf, "0123456789\r\nABCDEFGHIJ\r\n");

    Err error = get_line_from_buf(buf, offset_buf, len_buf, line, offset_line, &bytes_in_new_line);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(12, bytes_in_new_line);
    EXPECT_STREQ("0123456789\r\n", line);
}

TEST(pttdb_misc, get_line_from_buf_with_offset_buf) {
    int len_buf = 24;
    char buf[MAX_BUF_SIZE];
    char line[MAX_BUF_SIZE];
    int offset_buf = 12;
    int offset_line = 0;
    int bytes_in_new_line = 0;
    bzero(line, sizeof(line));

    strcpy(buf, "0123456789\r\nABCDEFGHIJ\r\n");

    Err error = get_line_from_buf(buf, offset_buf, len_buf, line, offset_line, &bytes_in_new_line);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(12, bytes_in_new_line);
    EXPECT_STREQ("ABCDEFGHIJ\r\n", line);
}

TEST(pttdb_misc, get_line_from_buf_with_line_offset) {
    int len_buf = 24;
    char buf[MAX_BUF_SIZE];
    char line[MAX_BUF_SIZE];
    int offset_buf = 0;
    int offset_line = 2;
    int bytes_in_new_line = 0;
    bzero(line, sizeof(line));

    strcpy(buf, "0123456789\r\nABCDEFGHIJ\r\n");
    line[0] = '!';
    line[1] = '@';

    Err error = get_line_from_buf(buf, offset_buf, len_buf, line, offset_line, &bytes_in_new_line);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(12, bytes_in_new_line);
    EXPECT_STREQ("!@0123456789\r\n", line);
}

TEST(pttdb_misc, get_line_from_buf_not_end) {
    int len_buf = 10;
    char buf[MAX_BUF_SIZE];
    char line[MAX_BUF_SIZE];
    int offset_buf = 0;
    int offset_line = 0;
    int bytes_in_new_line = 0;
    bzero(line, sizeof(line));

    strcpy(buf, "0123456789");

    Err error = get_line_from_buf(buf, offset_buf, len_buf, line, offset_line, &bytes_in_new_line);
    EXPECT_EQ(S_ERR, error);
    EXPECT_EQ(10, bytes_in_new_line);
    EXPECT_STREQ("0123456789", line);
}

TEST(pttdb_misc, get_line_from_buf_offset_buf_not_end) {
    int len_buf = 13;
    char buf[MAX_BUF_SIZE];
    char line[MAX_BUF_SIZE];
    int offset_buf = 3;
    int offset_line = 0;
    int bytes_in_new_line = 0;
    bzero(line, sizeof(line));

    strcpy(buf, "A\r\n0123456789");

    Err error = get_line_from_buf(buf, offset_buf, len_buf, line, offset_line, &bytes_in_new_line);
    EXPECT_EQ(S_ERR, error);
    EXPECT_EQ(10, bytes_in_new_line);
    EXPECT_STREQ("0123456789", line);
}

TEST(pttdb_misc, get_line_from_buf_r_only) {
    int len_buf = 13;
    char buf[MAX_BUF_SIZE];
    char line[MAX_BUF_SIZE];
    int offset_buf = 0;
    int offset_line = 0;
    int bytes_in_new_line = 0;
    bzero(line, sizeof(line));

    strcpy(buf, "A\r0123456789");

    Err error = get_line_from_buf(buf, offset_buf, len_buf, line, offset_line, &bytes_in_new_line);
    EXPECT_EQ(S_ERR, error);
    EXPECT_EQ(13, bytes_in_new_line);
    EXPECT_STREQ("A\r0123456789", line);
}

TEST(pttdb_misc, get_line_from_buf_n_only) {
    int len_buf = 13;
    char buf[MAX_BUF_SIZE];
    char line[MAX_BUF_SIZE];
    int offset_buf = 0;
    int offset_line = 0;
    int bytes_in_new_line = 0;
    bzero(line, sizeof(line));

    strcpy(buf, "A\n0123456789");

    Err error = get_line_from_buf(buf, offset_buf, len_buf, line, offset_line, &bytes_in_new_line);
    EXPECT_EQ(S_ERR, error);
    EXPECT_EQ(13, bytes_in_new_line);
    EXPECT_STREQ("A\n0123456789", line);
}

TEST(pttdb_misc, get_line_from_buf_line_with_max_buf_size) {
    int len_buf = 22;
    char buf[MAX_BUF_SIZE] = {};
    char line[MAX_BUF_SIZE] = {};
    int offset_buf = 0;
    int offset_line = MAX_BUF_SIZE - 10;
    int bytes_in_new_line = 0;

    strcpy(buf, "01234567890123456789\r\n");

    for(int i = 0; i < MAX_BUF_SIZE - 10; i++) line[i] = 'A';

    Err error = get_line_from_buf(buf, offset_buf, len_buf, line, offset_line, &bytes_in_new_line);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(10, bytes_in_new_line);
    EXPECT_EQ(0, strncmp("0123456789", line + MAX_BUF_SIZE - 10, 10));
}

TEST(pttdb_misc, get_line_from_buf_partial_line_break) {
    int len_buf = 13;
    char buf[MAX_BUF_SIZE];
    char line[MAX_BUF_SIZE];
    int offset_buf = 0;
    int offset_line = 2;
    int bytes_in_new_line = 0;
    bzero(line, sizeof(line));

    strcpy(buf, "\n0123456789\r\n");
    strcpy(line, "!\r");

    Err error = get_line_from_buf(buf, offset_buf, len_buf, line, offset_line, &bytes_in_new_line);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1, bytes_in_new_line);
    EXPECT_STREQ("!\r\n", line);
}

TEST(pttdb_misc, get_line_from_buf_end_of_buf) {
    int len_buf = 12;
    char buf[MAX_BUF_SIZE];
    char line[MAX_BUF_SIZE];
    int offset_buf = 12;
    int offset_line = 0;
    int bytes_in_new_line = 0;
    bzero(line, sizeof(line));

    strcpy(buf, "0123456789\r\n");

    Err error = get_line_from_buf(buf, offset_buf, len_buf, line, offset_line, &bytes_in_new_line);
    EXPECT_EQ(S_ERR, error);
    EXPECT_EQ(0, bytes_in_new_line);
}

TEST(pttdb_misc, form_rand_list) {
    int *rand_list = NULL;
    Err error = form_rand_list(100, &rand_list);
    EXPECT_EQ(S_OK, error);

    for(int i = 0; i < 100; i++) {
        fprintf(stderr, "test_pttdb_misc.form_rand_list: (%d/%d): %d\n", i, 100, rand_list[i]);
    }

    int sum = 0;
    for(int i = 0; i < 100; i++) {
        sum += rand_list[i];
    }
    EXPECT_EQ(4950, sum);

    safe_free(rand_list);
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

    FD = open("log.test_pttdb_misc.err", O_WRONLY | O_CREAT | O_TRUNC, 0660);
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
