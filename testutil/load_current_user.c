#include "bbs.h"
#include "testutil.h"

int
load_current_user(const char *uid)
{
    // userid should be already normalized.
    int is_admin_only = 0;

#ifdef ADMIN_PORT
    // ----------------------------------------------------- PORT TESTING
    // XXX currently this does not work if we're using tunnel.
    is_admin_only = (listen_port == ADMIN_PORT);
#endif

    // ----------------------------------------------------- NEW ACCOUNT

#ifdef STR_REGNEW
    if (!is_admin_only && strcasecmp(uid, STR_REGNEW) == 0) {

# ifndef LOGINASNEW
	assert(false);
	exit(0);
# endif // !LOGINASNEW

	new_register();
	clear();
	mkuserdir(cuser.userid);
	reginit_fav();
    } else
#endif

    // ----------------------------------------------------- RECOVER ACCOUNT

#ifdef STR_RECOVER
    if (!is_admin_only && strcasecmp(uid, STR_RECOVER) == 0) {
	// Allow more time due to sending emails for verification code.
	alarm(1800);
	recover_account();
	// Should have exited.
	assert(false);
	exit(1);
    } else
#endif

    // --------------------------------------------------- GUEST ACCOUNT

#ifdef STR_GUEST
    if (!is_admin_only && strcasecmp(uid, STR_GUEST) == 0) {
	if (initcuser(STR_GUEST)< 1) exit (0) ;
	pwcuInitGuestPerm();
	// can we prevent mkuserdir() here?
	mkuserdir(cuser.userid);
    } else
#endif

    // ---------------------------------------------------- USER ACCOUNT
    {
	if (!cuser.userid[0] && initcuser(uid) < 1)
            exit(0);

        if (is_admin_only) {
            if (!HasUserPerm(PERM_SYSOP | PERM_BBSADM | PERM_BOARD |
                             PERM_ACCOUNTS | PERM_CHATROOM |
                             PERM_VIEWSYSOP | PERM_PRG)) {
                puts("\r\n權限不足，請換其它 port 連線。\r\n");
                exit(0);
            }
        }

#ifdef LOCAL_LOGIN_MOD
	LOCAL_LOGIN_MOD();
#endif
	if (strcasecmp(str_sysop, cuser.userid) == 0){
#ifdef NO_SYSOP_ACCOUNT
	    exit(0);
#else /* 自動加上各個主要權限 */
	    // TODO only allow in local connection?
	    pwcuInitAdminPerm();
#endif
	}
	/* 早該有 home 了, 不知道為何有的帳號會沒有, 被砍掉了? */
	mkuserdir(cuser.userid);
	logattempt(cuser.userid, ' ', login_start_time, fromhost);
	ensure_user_agreement_version();
    }

    // check multi user
    multi_user_check();
    return 1;
}
