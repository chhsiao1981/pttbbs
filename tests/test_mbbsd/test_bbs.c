#include "gtest/gtest.h"

TEST(bbs, is_file_owner) {
    fileheader_t fhdr = {};
    userec_t usr = {};

    sprintf(fhdr.owner, "owner1");
    sprintf(usr.userid, "owner1");
    int ret = is_file_owner(&fhdr, &usr);
    
    EXPECT_EQ(0, ret);
}

/**********
 * MAIN
 */
class MyEnvironment: public ::testing::Environment {
public:
    void SetUp();
    void TearDown();
};

void MyEnvironment::SetUp() {
}

void MyEnvironment::TearDown() {
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new MyEnvironment);

    return RUN_ALL_TESTS();
}
