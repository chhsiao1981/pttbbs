#include "gtest/gtest.h"
#include "bbs.h"

TEST(migrate_merge, migrate_1to3_get_line) {
    char buf[] = "testtesttest\r\n";
    char buf2[] = "testtest\r\ntesttesttest";
    char buf3[] = "testtest\r\ntesttesttest\r\n";
    char line[MIGRATE_MERGE_BUF_SIZE];
    int bytes_in_new_line;

    // the simplest test.
    printf("the simplest test\n");
    EXPECT_EQ(0, migrate_1to3_get_line(buf, 0, 14, line, 0, &bytes_in_new_line));
    EXPECT_EQ(14, bytes_in_new_line);
    EXPECT_EQ(0, strncmp(line, "testtesttest\r\n", 14));

    // test when reaching the end of the buf
    printf("test when reaching the end of the buf\n");
    EXPECT_EQ(-1, migrate_1to3_get_line(buf, 14, 14, line, 0, &bytes_in_new_line));
    EXPECT_EQ(0, bytes_in_new_line);

    // test from middle of the buf. and no ending.
    printf("test from middle of the buf. and no ending\n");
    EXPECT_EQ(-1, migrate_1to3_get_line(buf2, 10, 22, line, 0, &bytes_in_new_line));
    EXPECT_EQ(12, bytes_in_new_line);
    EXPECT_EQ(0, strncmp(line, "testtesttest", 12));

    // test from middle of the buf. and no ending.
    printf("test from middle of the buf. and with ending\n");
    EXPECT_EQ(0, migrate_1to3_get_line(buf3, 10, 24, line, 0, &bytes_in_new_line));
    EXPECT_EQ(14, bytes_in_new_line);
    EXPECT_EQ(0, strncmp(line, "testtesttest\n\n", 14));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}