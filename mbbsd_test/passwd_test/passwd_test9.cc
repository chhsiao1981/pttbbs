#include "passwd_test.h"

TEST_F(PasswdTest, pwcuSaveViolateLaw) {
  // load SYSOP2
  initcuser("SYSOP2\0");
}
