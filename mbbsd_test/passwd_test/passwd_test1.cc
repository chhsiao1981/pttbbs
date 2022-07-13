#include "passwd_test.h"

TEST_F(PasswdTest, initcuser) {
  // load SYSOP2
  initcuser("SYSOP2\0");

  EXPECT_STREQ(cuser.userid, "SYSOP2\0");
}
