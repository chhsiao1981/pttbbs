#pragma once

#include "user_handle.hpp"

namespace emailchallenge {

bool EmailChallenge(bool check_input_email, const std::string &input_email, const userhandle::UserHandle &user, const int y, const std::string &prompt, const std::string &ip, const std::string &filename, int *out_y);

} // namespace
