#include <gtest/gtest.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "bbs.h"
#include "testutil.h"

// fav-load
class FavLoadTest : public ::testing::Test {
 protected:
  void SetUp() override {
    filename = NULL;

    system("pwd");
    system("cp -R ./testcase/home1 ./testcase/home");
    system("cp ./testcase/.PASSWDS1 ./testcase/.PASSWDS");
    system("cp ./testcase/.BRD1 ./testcase/.BRD");
    chdir(BBSHOME);
    load_uhash();
  }

  void TearDown() override {
    system("rm -r ./testcase/home");
    system("rm ./testcase/.PASSWDS");
    system("rm ./testcase/.BRD");
  }


  char *filename;
};

TEST_F(FavLoadTest, Basic) {
  // Expect two strings not to be equal.
  fav_load();

  EXPECT_EQ(NULL, filename);
}
