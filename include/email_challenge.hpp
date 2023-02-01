#pragma once

extern "C" {
#include "bbs.h"
#include "daemons.h"
}

#include <optional>
#include <string>
#include <vector>

#include "user_handle.hpp"

namespace email_challenge {

void LoadUserEmail(const userec_t *u, std::vector<std::string> &all_emails_);
bool LoadVerifyDbEmail(const std::optional<user_handle::UserHandle> &user_,
                       std::vector<std::string> &all_emails_);
void EmailCodeChallenge(const bool check_email_,
                        const std::string &email_,
                        const std::vector<std::string> &all_emails_,
                        const std::string &prompt,
                        const std::string &ip,
                        const std::string &filename,
                        int &y_);

} // namespace email_challenge
