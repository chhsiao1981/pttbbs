#include "gtest/gtest.h"
#include "test.h"
#include "cmpttui/pttui_thread_lock.h"
#include "cmpttui/pttui_thread_lock_private.h"

TEST(pttui_thread_lock, InitPttUIThreadLock)
{
    Err error_code = InitPttUIThreadLock();
    EXPECT_EQ(S_OK, error_code);

    error_code = DestroyPttUIThreadLock();
    EXPECT_EQ(S_OK, error_code);
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
    FD = open("log.cmcmpttui_thread_lock_test_pttui_thread_lock.err", O_WRONLY | O_CREAT | O_TRUNC, 0660);
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
