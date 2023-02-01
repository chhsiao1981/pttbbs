extern "C" {
#include "bbs.h"
#include "daemons.h"
}

#include <optional>
#include <string>
#include <vector>

#ifdef USE_VERIFYDB_ACCOUNT_RECOVERY
#include "verifydb.h"
#include "verifydb.fbs.h"
#endif // USE_VERIFYDB_ACCOUNT_RECOVERY

#include "user_handle.hpp"
#include "email_challenge.hpp"

namespace email_challenge {

constexpr int kMaxErrCnt = 3;
constexpr size_t kCodeLen = 30;

// static
std::optional<std::string>
NormalizeEmail(const std::string &email) {
  auto out = email;
  for (auto& c : out)
    c = std::tolower(c);
  auto idx = out.find('@');
  if (idx == std::string::npos)
    return std::nullopt;
  if (idx != out.rfind('@'))
    return std::nullopt;
  return out;
}

void LoadUserEmail(const userec_t *u, std::vector<std::string> &all_emails_) {
#ifdef USEREC_EMAIL_IS_CONTACT
  auto email = NormalizeEmail(u->email);
  if (email)
    all_emails_.push_back(email.value());
#endif // USEREC_EMAIL_IS_CONTACT
}

bool LoadVerifyDbEmail(const std::optional<user_handle::UserHandle> &user_,
                       std::vector<std::string> &all_emails_) {
#ifdef USE_VERIFYDB_ACCOUNT_RECOVERY
  Bytes buf;
  const VerifyDb::GetReply *reply;
  if (!verifydb_getuser(user_->userid.c_str(), user_->generation, &buf, &reply))
    return false;

  if (reply->entry()) {
    for (const auto *ent : *reply->entry()) {
      if (ent->vmethod() == VMETHOD_EMAIL && ent->vkey() != nullptr) {
        auto email = NormalizeEmail(ent->vkey()->str());
        if (email)
          all_emails_.push_back(email.value());
      }
    }
  }
#endif // USE_VERIFYDB_ACCOUNT_RECOVERY
  return true;
}

// static
bool SendRecoveryCode(const std::string &email,
                      const std::string &code,
                      const std::string &prompt,
                      const std::string &ip,
                      const std::string &filename) {
  std::string subject;
  subject.append(prompt);
  subject.append(" [ ");
  subject.append(code);
  subject.append(" ] @ IP ");
  subject.append(ip);
  bsmtp(filename.c_str(), subject.c_str(), email.c_str(), "non-exist");
  return true;
}

// static
std::string GenCode(size_t len) {
  std::string s(len, '\0');
  random_text_code(&s[0], s.size());
  return s;
}

// static
void UserErrorExit() {
  vmsg("錯誤次數過多，請重新操作。");
  exit(0);
}

void EmailCodeChallenge(const bool check_email_,
                        const std::string &email_,
                        const std::vector<std::string> &all_emails_,
                        const std::string &prompt,
                        const std::string &ip,
                        const std::string &filename,
                        int &y_) {
  // Generate code.
  auto code = GenCode(kCodeLen);

  y_++;
  mvprints(y_, 0, "系統處理中...");
  doupdate();

  // Check and send recovery code. Don't exit if email doesn't match, so user
  // can't guess email.
  bool email_matches = false;
  if (check_email_) {
    for (const auto &email : all_emails_) {
      if (email == email_)
        email_matches = true;
    }

    if (email_matches) {
      // We only want to send email if the user input the matching address.
      // Silently fail if email is not matching, so that user cannot guess other
      // user's email address.
      SendRecoveryCode(email_, code, prompt, ip, filename);
    }
  } else {
    email_matches = true;
    // We send to all the valid user emails.
    for (const auto &each_email : all_emails_) {
      SendRecoveryCode(each_email, code, prompt, ip, filename);
    }
  }

  // Add a random 5-10s delay to prevent timing oracle.
  usleep(5000000 + random() % 5000000);
  if (check_email_) {
    mvprints(y_++, 0, "若您輸入的資料正確，系統已將認證碼寄送至您的信箱。");
  } else {
    mvprints(y_++, 0, "系統已將認證碼寄送至您的信箱。");
  }

  // Input code.
  char incode[kCodeLen + 1] = {};
  int errcnt = 0;
  while (1) {
    getdata_buf(y_, 0, "請收信後輸入認證碼：", incode, sizeof(incode), DOECHO);
    if (email_matches && code == std::string(incode))
      break;
    if (++errcnt >= kMaxErrCnt)
      UserErrorExit();
    mvprints(y_ + 1, 0, "認證碼錯誤！認證碼共有 %d 字元。", (int)kCodeLen);
    incode[0] = '\0';
  }
  y_++;
  move(y_, 0);
  clrtoeol(); // There might be error message at this line.

  // Some paranoid checkings, we are about to let user reset their password.
  if (code.size() != kCodeLen || code != std::string(incode)) {
    assert(false);
    exit(1);
  }
}

} // namespace email_challenge

int email_code_challenge(const char *email,
                    const userec_t *u,
                    const int y,
                    const char *prompt,
                    const char *ip,
                    const char *filename,
                    int *out_y) {
  std::string email_str = {};
  bool check_email = false;
  if (email != NULL) {
    email_str = std::string(email);
    check_email = true;
  }

  user_handle::UserHandle user = {};
  if (!user_handle::InitUserHandle(u, user)) {
    return -1;
  }

  std::vector<std::string> all_emails = {};
  std::optional<user_handle::UserHandle> user_opt = user;

  email_challenge::LoadUserEmail(u, all_emails);

  if (!email_challenge::LoadVerifyDbEmail(user_opt, all_emails)) {
    return -1;
  }

  int y_ = y;
  email_challenge::EmailCodeChallenge(check_email, email_str, all_emails, prompt, ip, filename, y_);
  if (out_y != NULL)
    *out_y = y_;

  return 0;
}
