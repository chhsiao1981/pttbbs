#include "passwd_test.h"

TEST_F(PasswdTest, pwcuInitZero) {
  // load SYSOP2
  initcuser("SYSOP2\0");
}
