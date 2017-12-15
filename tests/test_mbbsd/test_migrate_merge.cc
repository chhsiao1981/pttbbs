#include "gtest/gtest.h"
#include "bbs.h"

// XXX refer to bbs.c
enum {
    RECTYPE_GOOD,
    RECTYPE_BAD,
    RECTYPE_ARROW,

    RECTYPE_SIZE,
    RECTYPE_MAX     = RECTYPE_SIZE-1,
    RECTYPE_DEFAULT = RECTYPE_GOOD, // match traditional user behavior
};


TEST(migrate_merge, migrate_1to3_get_line_from_start_with_ending) {
    char buf[] = "testtesttest\r\n\0";
    char line[MIGRATE_MERGE_BUF_SIZE];
    int bytes_in_new_line;

    bzero(line, sizeof(line));
    EXPECT_EQ(0, migrate_1to3_get_line(buf, 0, 14, line, 0, &bytes_in_new_line));
    EXPECT_EQ(14, bytes_in_new_line);
    EXPECT_EQ(0, strncmp(line, "testtesttest\r\n", 14));
}

TEST(migrate_merge, migrate_1to3_get_line_reaching_ending) {
    char buf[] = "testtesttest\r\n\0";
    char line[MIGRATE_MERGE_BUF_SIZE];
    int bytes_in_new_line;

    bzero(line, sizeof(line));
    EXPECT_EQ(-1, migrate_1to3_get_line(buf, 14, 14, line, 0, &bytes_in_new_line));
    EXPECT_EQ(0, bytes_in_new_line);
}

TEST(migrate_merge, migrate_1to3_get_line_from_middle_no_ending) {
    char buf[] = "testtest\r\ntesttesttest\0";
    char line[MIGRATE_MERGE_BUF_SIZE];
    int bytes_in_new_line;

    bzero(line, sizeof(line));
    EXPECT_EQ(-1, migrate_1to3_get_line(buf, 10, 22, line, 0, &bytes_in_new_line));
    EXPECT_EQ(12, bytes_in_new_line);
    EXPECT_EQ(0, strncmp(line, "testtesttest", 12));
}

TEST(migrate_merge, migrate_1to3_get_line_from_middle_with_ending) {
    char buf[] = "testtest\r\ntesttesttest\r\n\0";
    char line[MIGRATE_MERGE_BUF_SIZE];
    int bytes_in_new_line;

    bzero(line, sizeof(line));
    EXPECT_EQ(0, migrate_1to3_get_line(buf, 10, 24, line, 0, &bytes_in_new_line));
    EXPECT_EQ(14, bytes_in_new_line);
    EXPECT_EQ(0, strncmp(line, "testtesttest\r\n", 14));
}

TEST(migrate_merge, migrate_1to3_get_line_with_r_in_line) {
    char buf[] = "\ntesttesttest\r\n\0";
    char line[MIGRATE_MERGE_BUF_SIZE];
    int bytes_in_new_line;

    bzero(line, sizeof(line));
    line[0] = '\r';
    EXPECT_EQ(0, migrate_1to3_get_line(buf, 0, 15, line, 1, &bytes_in_new_line));
    EXPECT_EQ(1, bytes_in_new_line);
    EXPECT_EQ(0, strncmp(line, "\r\n", 2));
}

TEST(migrate_merge, migrate_1to3_is_recommend_line_match) {
    char buf[MIGRATE_MERGE_BUF_SIZE];
    int bytes_in_line = 20;

    FormatCommentString(buf, MIGRATE_MERGE_BUF_SIZE, RECTYPE_GOOD, "testid", 20, "testmsg", "02/10");
    printf("buf: %s GOOD: %d BAD %d ARROW: %d\n", buf, RECTYPE_GOOD, RECTYPE_BAD, RECTYPE_ARROW);
    for(int i = 0; i < strlen(buf); i++) printf("buf: (%d/%d)\n", i, buf[i]);

    EXPECT_EQ(YEA, migrate_1to3_is_recommend_line(buf, bytes_in_line));
}

TEST(migrate_merge, migrate_1to3_is_recommend_line_not_match) {
    char buf[] = "testtesttesttesttest";
    int bytes_in_line = 20;
    EXPECT_EQ(NA, migrate_1to3_is_recommend_line(buf, bytes_in_line));
}

TEST(migrate_merge, migrate_1to3_is_recommend_line_not_enough_length) {
    char buf[] = "testtesttest";
    int bytes_in_line = 12;
    EXPECT_EQ(NA, migrate_1to3_is_recommend_line(buf, bytes_in_line));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
