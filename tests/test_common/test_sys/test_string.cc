#include "gtest/gtest.h"
#include "fnv_hash.h"

#include "ansi.h"
#include "cmsys.h"

TEST(sys/string, CHAR_LOWER) {
    EXPECT_EQ('a', CHAR_LOWER('A'));
    EXPECT_EQ('0', CHAR_LOWER('0'));
    EXPECT_EQ('-', CHAR_LOWER('-'));
    EXPECT_EQ('b', CHAR_LOWER('b'));
    EXPECT_EQ('[', CHAR_LOWER('['));
    EXPECT_EQ('{', CHAR_LOWER('{'));
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
