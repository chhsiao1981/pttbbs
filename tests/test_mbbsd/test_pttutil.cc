#include "gtest/gtest.h"
#include "bbs.h"

TEST(pttutil, get_line_from_buf) {
    int len_buf = 24;
    char buf[MAX_BUF_SIZE];
    char line[MAX_BUF_SIZE];
    int offset_line = 0;
    int bytes_in_new_line = 0;

    strcpy(buf, "0123456789\r\n0123456789\r\n");
    Err error = get_line_from_buf(buf, 0, len_buf, line, offset_line, &bytes_in_new_line);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(12, bytes_in_new_line);
    EXPECT_STREQ("0123456789\r\n", line);
}
