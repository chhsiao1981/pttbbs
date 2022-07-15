#include "passwd_test.h"

TEST_F(PasswdTest, pwcuCancelBadpost) {
  // load SYSOP2
  initcuser("SYSOP2\0");
}
