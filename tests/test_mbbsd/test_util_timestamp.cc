#include "gtest/gtest.h"
#include "util_timestamp.h"

TEST(util_timestamp, milli_timestamp_to_year) {
    int year = 0;
    // 2018-03-28 00:58:29
    Err error = milli_timestamp_to_year(1522169909000, &year);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(year, 2018);
}

TEST(util_timestamp, milli_timestamp_to_timestamp) {
    time64_t timestamp = 0;
    Err error = milli_timestamp_to_timestamp(1522169909123, &timestamp);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1522169909, timestamp);
}

TEST(util_timestamp, datetime_to_timestamp) {
    time64_t timestamp = 0;
    Err error = datetime_to_timestamp(2018, 1, 1, 0, 0, 0, TIMEZONE_TAIPEI, &timestamp);

    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1514736000, timestamp);

    error = datetime_to_timestamp(2017, 5, 3, 13, 49, 38, TIMEZONE_TAIPEI, &timestamp);

    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1493790578, timestamp);
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
    FD = open("log.test_util_timestamp.err", O_WRONLY | O_CREAT | O_TRUNC, 0660);
    dup2(FD, 2);
}

void MyEnvironment::TearDown() {
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
