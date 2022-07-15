#include "passwd_test.h"

TEST_F(PasswdTest, pwcuInitGuestPerm) {
  // load SYSOP2
  initcuser("SYSOP2\0");
}
