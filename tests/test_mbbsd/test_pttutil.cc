#include "gtest/gtest.h"
#include "bbs.h"

TEST(pttutil, get_line_from_buf) {
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

TEST(pttutil, get_line_from_buf_with_offset_buf) {
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

TEST(pttutil, get_line_from_buf_with_line_offset) {
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

TEST(pttutil, get_line_from_buf_not_end) {
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

TEST(pttutil, get_line_from_buf_offset_buf_not_end) {
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

TEST(pttutil, get_line_from_buf_partial_line_break) {
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

TEST(pttutil, get_line_from_buf_end_of_buf) {
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
