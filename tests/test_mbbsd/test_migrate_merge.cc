#include "gtest/gtest.h"
#include "bbs.h"

TEST(migrate_merge, mirgate_1to3_get_line) {
    char buf[] = "testtesttest\r\n";
    char line[MIGRATE_MERGE_BUF_SIZE];
    int bytes_in_new_line;

    EXPECT_EQ(0, mirgate_1to3_get_line(buf, 0, 14, line, 0, &bytes_in_new_line));
    EXPECT_EQ(14, bytes_in_new_line);
    EXPECT_EQ(0, strncmp(line, "testtesttest\r\n", 14));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}