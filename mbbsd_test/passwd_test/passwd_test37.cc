#include "passwd_test.h"

TEST_F(PasswdTest, pwcuInitAdminPerm) {
  // load SYSOP2
  initcuser("SYSOP2\0");
}
