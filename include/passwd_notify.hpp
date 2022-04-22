#pragma once

#include <string>

namespace passwdnotify {

class PasswdNotify {
public:
    static void NotifyMe();
    static void NotifyUser(const std::string &userid, const std::string &fromhost, const std::string &email);
};

} //namespace
