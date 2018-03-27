#include "gtest/gtest.h"
#include "util_timestamp.h"

TEST(util_teimstamp, milli_timestamp_to_year) {
    int year = 0;
    Err error = milli_timestamp_to_year(1522169909000, &year);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(year, 2018);
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
