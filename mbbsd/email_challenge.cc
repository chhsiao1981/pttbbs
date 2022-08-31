extern "C" {
#include "bbs.h"
#include "daemons.h"
}

#include <optional>
#include <string>

#include "verifydb.h"
#include "verifydb.fbs.h"

#include "email_challenge.hpp"
#include "user_handle.hpp"

// We don't use #ifdef USE_2FA or #ifdef USE_VERIFYDB_ACCOUNT_RECOVERY
// for the whole namespace because this is used in multiple situations.
// In addition, this is compiled as a separated .o file. A good compiler
// should be able to skip this .o when linking.
namespace emailchallenge {

constexpr int kMaxErrCnt = 3;
constexpr size_t kCodeLen = 30;

static void LoadUserEmail(const userhandle::UserHandle &user, std::vector<std::string> &all_emails);
static bool LoadVerifyDbEmail(const userhandle::UserHandle &user, std::vector<std::string> &all_emails);
static bool AppendNonRepeatedEmail(const std::optional<std::string> &email, std::vector<std::string> &all_emails);
static std::optional<std::string> NormalizeEmail(const std::string &email);
static std::string GenCode(size_t len);
static bool SendChallengeCode(const std::string &email, const std::string &code, const std::string &prompt, const std::string &ip, const std::string &filename);
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
// params:
//     check_input_email: whether to check input_email (AccountRecovery) or not (reset password / change contact email).
//                       It's possible that input_email is "" even if we do want to check input email.
//                       We can't put input_email as NULL to indicate that we want to skip checking input email.
//     input_email: user-input-email
//     user: user
//     y: starting y onscreen
//     prompt: prompt for the email title. (prompt[ code ]@ip)
//     ip: ip for the email title.
//     filename: filename for the email template.
//
// return:
//     bool: true: success / false: fail
//     out_y: cursor-y after returning from the function.
bool EmailChallenge(bool check_input_email, const std::string &input_email, const userhandle::UserHandle &user, const int y, const std::string &prompt, const std::string &ip, const std::string &filename, int *out_y) {
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
  int current_y = y;
  current_y++;
  mvprints(current_y, 0, "正在寄認證碼到您的信箱裡...");

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
    mvprints(current_y++, 0, "若您輸入的資料正確，系統已將認證碼寄送至您的信箱。");
  } else {
    mvprints(current_y++, 0, "系統已將認證碼寄送至您的信箱。");
  }

  /////
  // 5. Input code.
  /////
  char incode[kCodeLen + 1] = {};
  int errcnt = 0;
  while (1) {
    getdata_buf(current_y, 0, "請收信後輸入認證碼：", incode, sizeof(incode), DOECHO);
    if (email_matches && code == std::string(incode)) {
      break;
    }
    if (++errcnt >= kMaxErrCnt) {
      UserErrorExit();
    }
    mvprints(current_y + 1, 0, "認證碼錯誤！認證碼共有 %d 字元。", (int)kCodeLen);
    incode[0] = '\0';
  }
  current_y++;
  move(current_y, 0);
  clrtoeol(); // There might be error message at this line.

  /////
  // 6. Some paranoid checkings. we are about to let user pass email-challenge.
  /////
  if (code.size() != kCodeLen || code != std::string(incode)) {
    assert(false);
    exit(1);
  }

  if (out_y != NULL) {
    *out_y = current_y;
  }

  return true;
}

static void LoadUserEmail(const userhandle::UserHandle &user, std::vector<std::string> &all_emails) {
#ifdef USEREC_EMAIL_IS_CONTACT
  std::optional<std::string> normalized_contact_email_opt = NormalizeEmail(user.email);
  std::string normalized_contact_email = {};
  if (normalized_contact_email_opt) {
    normalized_contact_email = normalized_contact_email_opt.value();
  }
  // append only if contact-email is valid.
  if (normalized_contact_email_opt) {
    AppendNonRepeatedEmail(normalized_contact_email, all_emails);
  }
#endif //USEREC_EMAIL_IS_CONTACT
}

static bool LoadVerifyDbEmail(const userhandle::UserHandle &user, std::vector<std::string> &all_emails) {
#ifdef USE_VERIFYDB_ACCOUNT_RECOVERY
# ifndef USE_VERIFYDB
#   error "USE_VERIFYDB_ACCOUNT_RECOVERY requires USE_VERIFYDB"
# endif // USE_VERIFYDB

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
        AppendNonRepeatedEmail(email, all_emails);
      }
    }
  }
#endif // USE_VERIFYDB_ACCOUNT_RECOVERY

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
static bool AppendNonRepeatedEmail(const std::optional<std::string> &email, std::vector<std::string> &all_emails) {
  if (!email) {
    return false;
  }

  bool is_match = false;
  for (const auto &each_email : all_emails) {
    if (each_email == email) {
      is_match = true;
      break;
    }
  }
  if (!is_match) {
    all_emails.push_back(email.value());
    return true;
  }

  return false;
}

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

static std::string GenCode(size_t len) {
  std::string s(len, '\0');
  random_text_code(&s[0], s.size());
  return s;
}

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
//     input_email: input email. NULL if directly sending to the contact email (user.email) without checking.
//     user: user
//     y: starting y onscreen
//     prompt: prompt for the email title. (prompt[ code ]@ip)
//     ip: ip for the email title.
//     filename: filename for the email template.
//
// Return:
//     int: non-zero: success, 0: failed
//     out_y: cursor-y after returning from the function.
int email_challenge(const char *input_email, const userec_t *user, const int y, const char *prompt, const char *ip, const char *filename, int *out_y) {
  std::string input_email_str = {};
  bool check_input_email = false;
  if (input_email != NULL) {
    input_email_str = std::string(input_email);
    check_input_email = true;
  }

  userhandle::UserHandle user_handle = {};
  bool ret = userhandle::InitUserHandle(user, user_handle);
  if (!ret) {
    return 0;
  }

  return emailchallenge::EmailChallenge(check_input_email, input_email_str, user_handle, y, prompt, ip, filename, out_y);
}
