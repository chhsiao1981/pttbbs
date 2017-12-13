#include "gtest/gtest.h"
#include "bbs.h"

TEST(migrate_merge, migrate_1to3_get_line) {
    char buf[] = "testtesttest\r\n\0";
    char line[MIGRATE_MERGE_BUF_SIZE];
    int bytes_in_new_line;

    // the simplest test.
    EXPECT_EQ(0, migrate_1to3_get_line(buf, 0, 14, line, 0, &bytes_in_new_line));
    EXPECT_EQ(14, bytes_in_new_line);
    EXPECT_EQ(0, strncmp(line, "testtesttest\r\n", 14));

    // test when reaching the end of the buf
    bzero(line, sizeof(line));
    EXPECT_EQ(-1, migrate_1to3_get_line(buf, 14, 14, line, 0, &bytes_in_new_line));
    EXPECT_EQ(0, bytes_in_new_line);

}

TEST(migrate_merge, migrate_1to3_get_line_2) {
    char buf[] = "testtest\r\ntesttesttest\0";
    char line[MIGRATE_MERGE_BUF_SIZE];
    int bytes_in_new_line;

    // test from middle of the buf. and no ending.
    bzero(line, sizeof(line));
    EXPECT_EQ(-1, migrate_1to3_get_line(buf, 10, 22, line, 0, &bytes_in_new_line));
    EXPECT_EQ(12, bytes_in_new_line);
    EXPECT_EQ(0, strncmp(line, "testtesttest", 12));
}

TEST(migrate_merge, migrate_1to3_get_line_3) {
    char buf[] = "testtest\r\ntesttesttest\r\n\0";
    char line[MIGRATE_MERGE_BUF_SIZE];
    int bytes_in_new_line;

    // test from middle of the buf. and with ending.
    bzero(line, sizeof(line));
    EXPECT_EQ(0, migrate_1to3_get_line(buf, 10, 24, line, 0, &bytes_in_new_line));
    EXPECT_EQ(14, bytes_in_new_line);
    EXPECT_EQ(0, strncmp(line, "testtesttest\r\n", 14));
}

TEST(migrate_merge, migrate_1to3_get_line_4) {
    char buf[] = "\ntesttesttest\r\n\0";
    char line[MIGRATE_MERGE_BUF_SIZE];
    int bytes_in_new_line;

    // test from middle of the buf. and with ending.
    bzero(line, sizeof(line));
    line[0] = '\r';
    EXPECT_EQ(0, migrate_1to3_get_line(buf, 0, 15, line, 1, &bytes_in_new_line));
    EXPECT_EQ(1, bytes_in_new_line);
    EXPECT_EQ(0, strncmp(line, "\r\n", 2));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
