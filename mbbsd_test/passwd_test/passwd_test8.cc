#include "passwd_test.h"

TEST_F(PasswdTest, pwcuViolateLaw) {
  // load SYSOP2
  initcuser("SYSOP2\0");
}
