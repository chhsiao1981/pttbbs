extern "C" {
#include "bbs.h"
#include "daemons.h"
}

#ifdef USE_2FA
# ifndef USE_VERIFYDB_ACCOUNT_RECOVERY
#   error "USE_2FA requires USE_VERIFYDB_ACCOUNT_RECOVERY and USE_VERIFYDB"
# endif // USE_VERIFYDB_ACCOUNT_RECOVERY
#endif // USE_2FA

#ifdef USE_VERIFYDB_ACCOUNT_RECOVERY

# ifndef USE_VERIFYDB
#   error "USE_VERIFYDB_ACCOUNT_RECOVERY requires USE_VERIFYDB"
# endif // USE_VERIFYDB

#include <optional>
#include <string>

#include "verifydb.h"
#include "verifydb.fbs.h"

#include "user_handle.hpp"
#include "email_challenge.hpp"

namespace emailchallenge {

constexpr int kMaxErrCnt = 3;
constexpr size_t kCodeLen = 30;

static void LoadUserEmail(const userhandle::UserHandle &user, std::vector<std::string> &all_emails);
static bool LoadVerifyDbEmail(const userhandle::UserHandle &user, std::vector<std::string> &all_emails);
static bool AppendNonRepeatedEmail(const std::string &email, std::vector<std::string> &all_emails);
static std::optional<std::string> NormalizeEmail(const std::string &email);
static bool SendChallengeCode(const std::string &email,
                              const std::string &code,
                              const std::string &prompt,
                              const std::string &ip,
                              const std::string &filename);
static std::string GenCode(size_t len);
static void UserErrorExit();

// EmailChallenge
//
// Challenge user by sending randomized-code to the user's email.
// Expected to compatible with AccountRecovery::EmailCodeChallenge,
// which challenge with both contact-email and verify-email.
//
// However, there are scenarios that the user does not provide the input-email.
// We send to the user's contact email in these scenarios.
//
// ref: AccountRecovery::EmailCodeChallenge
//
// params:
//   check_input_email: whether to check input_email (AccountRecovery) or not (reset password / change contact email).
//                      It's possible that input_email is "" even if we do want to check input email.
//                      We can't put input_email as NULL to indicate that we want to skip checking input email.
//   input_email: user-input-email
//   user: user
//   y: starting y onscreen
//   prompt: prompt for the email title. (prompt[ code ]@ip)
//   ip: ip for the email title.
//   filename: filename for the email template.
//
// return:
//   bool: true: success / false: fail
//   out_y: if not NULL: cursor-y after returning from the function.
bool EmailChallenge(const bool check_input_email,
                    const std::string &input_email,
                    const userhandle::UserHandle &user,
                    const int y,
                    const std::string &prompt,
                    const std::string &ip,
                    const std::string &filename,
                    int *out_y) {
  /////
  // 1. Load all emails
  /////
  std::vector<std::string> all_emails = {};

  // load contact email
  // ref: AccountRecovery::LoadUser
  LoadUserEmail(user, all_emails);

  // load verify-db email (regemail).
  // ref: AccountRecovery::InputUserEmail
  if (!LoadVerifyDbEmail(user, all_emails)) {
    vmsg("系統錯誤，請稍候再試。");
    exit(0);
  }

  /////
  // 2. GenCode
  //    ref: AccountRecovery::EmailCodeChallenge
  /////
  auto code = GenCode(kCodeLen);

  /////
  // 2.1. prompt
  /////
  int y_ = y; // can't do y++ because y is const.
  y_++;
  mvprints(y_, 0, "正在寄認證碼到您的信箱裡...");

  doupdate();

  /////
  // 3. Check and send challenge code. Do not exit if email doesn't match,
  //    so user can't guess email.
  /////
  bool email_matches = false;
  if (check_input_email) {
    for (const auto &each_email : all_emails) {
      if (each_email == input_email) {
        email_matches = true;
      }
    }

    if (email_matches) {
      // We only want to send email if the user input the matching address.
      // Silently fail if email is not matching, so that user cannot guess other
      // user's email address.
      //
      // normalized_email is guaranteed to be a valid email.
      SendChallengeCode(input_email, code, prompt, ip, filename);
    }
  } else {
    email_matches = true;
    // We would like to send to all the valid user emails.
    for (const auto &each_email : all_emails) {
      SendChallengeCode(each_email, code, prompt, ip, filename);
    }
  }

  /////
  // 4. Add a random 5-10s delay to prevent timing oracle.
  /////
  usleep(5000000 + random() % 5000000);
  if (check_input_email) {
    mvprints(y_++, 0, "若您輸入的資料正確，系統已將認證碼寄送至您的信箱。");
  } else {
    mvprints(y_++, 0, "系統已將認證碼寄送至您的信箱。");
  }

  /////
  // 5. Input code.
  /////
  char incode[kCodeLen + 1] = {};
  int errcnt = 0;
  while (1) {
    getdata_buf(y_, 0, "請收信後輸入認證碼：", incode, sizeof(incode), DOECHO);
    if (email_matches && code == std::string(incode)) {
      break;
    }
    if (++errcnt >= kMaxErrCnt) {
      UserErrorExit();
    }
    mvprints(y_ + 1, 0, "認證碼錯誤！認證碼共有 %d 字元。", (int)kCodeLen);
    incode[0] = '\0';
  }
  y_++;
  move(y_, 0);
  clrtoeol(); // There might be error message at this line.

  /////
  // 6. Some paranoid checkings. we are about to let user pass email-challenge.
  /////
  if (code.size() != kCodeLen || code != std::string(incode)) {
    assert(false);
    exit(1);
  }

  if (out_y != NULL) {
    *out_y = y_;
  }

  return true;
}

// LoadUserEmail
// ref: the last part in AccountRecovery::LoadUser
static void LoadUserEmail(const userhandle::UserHandle &user, std::vector<std::string> &all_emails) {
#ifdef USEREC_EMAIL_IS_CONTACT
  auto email = NormalizeEmail(user.email);
  if (email) {
    AppendNonRepeatedEmail(email.value(), all_emails);
  }
#endif //USEREC_EMAIL_IS_CONTACT
}

// LoadVerifyDbEmail
// ref: AccountRecovery::LoadVerifyDbEmail
static bool LoadVerifyDbEmail(const userhandle::UserHandle &user, std::vector<std::string> &all_emails) {
  Bytes buf = {};
  // reply is pointing to buf. no need to free reply in the end.
  const VerifyDb::GetReply *reply = NULL;
  if (!verifydb_getuser(user.userid.c_str(), user.generation, &buf, &reply)) {
    return false;
  }

  if (reply->entry()) {
    for (const auto *ent : *reply->entry()) {
      if (ent->vmethod() == VMETHOD_EMAIL && ent->vkey() != nullptr) {
        auto email = NormalizeEmail(ent->vkey()->str());
        if (email) {
          AppendNonRepeatedEmail(email.value(), all_emails);
        }
      }
    }
  }
  return true;
}

// AppendNonRepeatedEmail
//
// Append only non-repeated emails.
// It is expected that there are only 2-3 emails.
// It should be ok that we simply use for-loop
// to check non-repeated emails.
//
// Return:
//   bool: true: successfully appended / false: no append
//   all_emails: appended email list.
static bool AppendNonRepeatedEmail(const std::string &email, std::vector<std::string> &all_emails) {
  bool is_match = false;
  for (const auto &each_email : all_emails) {
    if (each_email == email) {
      is_match = true;
      break;
    }
  }
  if (!is_match) {
    all_emails.push_back(email);
    return true;
  }

  return false;
}

// NormalizeEmail
// ref: AccountRecovery::NormalizeEmail
static std::optional<std::string> NormalizeEmail(const std::string &email) {
  auto out = email;
  for (auto& c : out) {
    c = std::tolower(c);
  }
  auto idx = out.find('@');
  if (idx == std::string::npos) {
    return std::nullopt;
  }
  if (idx != out.rfind('@')) {
    return std::nullopt;
  }
  return out;
}

// SendChallengeCode
// ref: AccountRecovery::SendRecoveryCode
static bool SendChallengeCode(const std::string &email,
                              const std::string &code,
                              const std::string &prompt,
                              const std::string &ip,
                              const std::string &filename) {
  std::string subject;
  subject.append(prompt);
  subject.append("[ ");
  subject.append(code);
  subject.append(" ]@");
  subject.append(ip);
  bsmtp(filename.c_str(), subject.c_str(), email.c_str(), "non-exist");
  return true;
}

// GenCode
// ref: AccountRecovery::GenCode
static std::string GenCode(size_t len) {
  std::string s(len, '\0');
  random_text_code(&s[0], s.size());
  return s;
}

// UserErrorExit
// ref: AccountRecovery::UserErrorExit
static void UserErrorExit() {
  vmsg("錯誤次數過多，請重新操作。");
  exit(0);
}

} // namespace


// email_challenge
//
// Challenge user by sending randomized-code to the user's email.
//
// Params:
//   input_email: input email. NULL if directly sending to the contact email (user.email) without checking.
//   user: user
//   y: starting y onscreen
//   prompt: prompt for the email title. (prompt[ code ]@ip)
//   ip: ip for the email title.
//   filename: filename for the email template.
//
// Return:
//   int: non-zero: success, 0: failed
//   out_y: if not NULL: cursor-y after returning from the function.
int email_challenge(const char *input_email,
                    const userec_t *user,
                    const int y,
                    const char *prompt,
                    const char *ip,
                    const char *filename,
                    int *out_y) {
  std::string input_email_str = {};
  bool check_input_email = false;
  if (input_email != NULL) {
    input_email_str = std::string(input_email);
    check_input_email = true;
  }

  userhandle::UserHandle user_handle = {};
  if (!userhandle::InitUserHandle(user, user_handle)) {
    return 0;
  }

  return emailchallenge::EmailChallenge(check_input_email, input_email_str, user_handle, y, prompt, ip, filename, out_y);
}

#endif // USE_VERIFYDB_ACCOUNT_RECOVERY
