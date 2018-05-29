#define _XOPEN_SOURCE
#include "gtest/gtest.h"
#include "test.h"
#include "cmpttlib/util_time.h"
#include "cmpttlib/util_time_private.h"

// 2018-01-01
time64_t _START_MILLI_TIMESTAMP = 1514764800000;

// 2019-01-01
time64_t _END_MILLI_TIMESTAMP = 1546300800000;

TEST(util_time, GetMilliTimestamp)
{
    time64_t milli_timestamp = 0;
    Err error_code = GetMilliTimestamp(&milli_timestamp);
    EXPECT_EQ(S_OK, error_code);

    EXPECT_GT(milli_timestamp, _START_MILLI_TIMESTAMP);
    EXPECT_LT(milli_timestamp, _END_MILLI_TIMESTAMP);
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
    FD = open("log.cmcmutil_time_test_util_time.err", O_WRONLY | O_CREAT | O_TRUNC, 0660);
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
