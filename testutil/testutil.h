#pragma once

#ifdef __cplusplus
extern "C" {
#endif

  // load uhash
  void load_uhash();

  // load current user
  int load_current_user(const char *uid);

  // multi user check
  void multi_user_check(void);

#ifdef __cplusplus
}
#endif
