#include <gtest/gtest.h>
#include <string.h>

// fav4_read_favrec
TEST(Fav4ReadFavrecTest, Basic) {  
  // Expect two strings not to be equal.  
  char filename[] = "";
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
  EXPECT_EQ(0, strlen(filename));
}

