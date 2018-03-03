#include "gtest/gtest.h"

TEST(dummy, dummy) {
    EXPECT_EQ(0, 0);
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

    FD = open("log.test_dummy.err", O_WRONLY|O_CREAT|O_TRUNC, 0660);
    dup2(FD, 2);
}

void MyEnvironment::TearDown() {
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new MyEnvironment);

    return RUN_ALL_TESTS();
}
