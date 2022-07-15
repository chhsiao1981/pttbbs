#include "passwd_test.h"

TEST_F(PasswdTest, pwcuReload) {
  // load SYSOP2
  initcuser("SYSOP2\0");
}
