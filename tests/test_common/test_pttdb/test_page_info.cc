#include "gtest/gtest.h"
#include "test.h"
#include "cmpttdb/page_info.h"
#include "cmpttdb/page_info_private.h"

TEST(page_info, InitPageInfo)
{
    Err error_code = InitPageInfo();
    EXPECT_EQ(S_OK, error_code);

    error_code = DestroyPageInfo();
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
    FD = open("log.cmpage_info_test_page_info.err", O_WRONLY | O_CREAT | O_TRUNC, 0660);
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
