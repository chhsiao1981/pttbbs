extern "C" {
#include "bbs.h"
}

#include <string>
#include "user_handle.hpp"

namespace user_handle {

bool InitUserHandle(const userec_t *u, UserHandle &user) {
  if (u == NULL || !is_validuserid(u->userid)) {
    return false;
  }

  user.userid = u->userid;
  user.generation = u->firstlogin;

  return true;
}

} // namespace user_handle
