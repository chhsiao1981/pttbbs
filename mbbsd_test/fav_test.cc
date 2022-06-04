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
    printf("BBSHOME: " BBSHOME "\n");
    system("mkdir -p " BBSHOME);
    system("cp -R ./testcase/home1 " BBSHOME "/home");
    system("cp ./testcase/.PASSWDS1 " BBSHOME "/.PASSWDS");
    system("cp ./testcase/.BRD1 " BBSHOME "/.BRD");
    // chdir(BBSHOME);
    system("echo \"pwd:\" && pwd");
    load_uhash();
    load_current_user("SYSOP");
  }

  void TearDown() override {
    system("rm -r " BBSHOME "/home");
    system("rm  " BBSHOME "/.PASSWDS");
    system("rm " BBSHOME " /.BRD");
  }


  char *filename;
};

TEST_F(FavLoadTest, Basic) {
  // Expect two strings not to be equal.
  fav_load();

  EXPECT_EQ(NULL, filename);
}
