#include <gtest/gtest.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "bbs.h"

// fav-load
class FavLoadTest : public ::testing::Test {
 protected:
  void SetUp() override {
    filename = NULL;

    system("pwd");
    system("cp -R ./mbbsd_test/testcase/home1 ./mbbsd_test/testcase/home");
  }

  void TearDown() override {
    system("rm -r ./mbbsd_test/testcase/home");
  }

  char *filename;
};

TEST_F(FavLoadTest, Basic) {
  // Expect two strings not to be equal.
  fav_load();

  EXPECT_EQ(NULL, filename);
}
