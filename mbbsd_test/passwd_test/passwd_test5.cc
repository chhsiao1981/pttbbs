#include "passwd_test.h"

TEST_F(PasswdTest, pwcuDisableLevel) {
  // load SYSOP2
  initcuser("SYSOP2\0");
}
