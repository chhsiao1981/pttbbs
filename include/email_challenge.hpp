#pragma once

#ifdef USE_VERIFYDB_ACCOUNT_RECOVERY
namespace emailchallenge {
bool EmailChallenge(const std::string &email, const userec_t *user, const int y, const std::string &prompt, const std::string &ip, const std::string &filename, int *out_y);
}
#endif

