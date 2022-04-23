#pragma once

#include <string>

namespace passwdnotify {

static void NotifyMe();
static void NotifyUser(const std::string &userid, const std::string &fromhost, const std::string &email);

} //namespace
