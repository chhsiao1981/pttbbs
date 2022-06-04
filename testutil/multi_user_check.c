#include "bbs.h"
#include "testutil.h"

static userinfo_t*
getotherlogin(int num)
{
    userinfo_t *ui;
    do {
	if (!(ui = (userinfo_t *) search_ulistn(usernum, num)))
	    return NULL;		/* user isn't logged in */

	/* skip sleeping process, this is slow if lots */
	if(ui->mode == DEBUGSLEEPING)
	    num++;
	else if(ui->pid <= 0)
	    num++;
	else if(kill(ui->pid, 0) < 0)
	    num++;
	else
	    break;
    } while (1);

    return ui;
}

void
multi_user_check(void)
{
    register userinfo_t *ui;
    char            genbuf[3];

    if (HasUserPerm(PERM_SYSOP))
	return;			/* don't check sysops */

    // race condition here, sleep may help..?
    if (cuser.userlevel) {
	usleep(random()%1000000); // 0~1s
	ui = getotherlogin(1);
	if(ui == NULL)
	    return;

	move(b_lines-3, 0); clrtobot();
	outs("\n" ANSI_COLOR(1) "注意: 您有其它連線已登入此帳號。" ANSI_RESET);
	getdata(b_lines - 1, 0, "您想刪除其他重複登入的連線嗎？[Y/n] ",
		genbuf, 3, LCECHO);

	usleep(random()%5000000); // 0~5s
	if (genbuf[0] != 'n') {
	    do {
		// scan again, old ui may be invalid
		ui = getotherlogin(1);
		if(ui==NULL)
		    return;
		if (ui->pid > 0) {
		    if(kill(ui->pid, SIGHUP)<0) {
			perror("kill SIGHUP fail");
			break;
		    }
		    log_usies("KICK ", cuser.nickname);
		}  else {
		    fprintf(stderr, "id=%s ui->pid=0\n", cuser.userid);
		}
		usleep(random()%2000000+1000000); // 1~3s
	    } while(getotherlogin(3) != NULL);
	} else {
	    /* deny login if still have 3 */
	    if (getotherlogin(3) != NULL) {
		sleep(1);
		abort_bbs(0);	/* Goodbye(); */
	    }
	}
    } else {
	/* allow multiple guest user */
	if (search_ulistn(usernum, MAX_GUEST) != NULL) {
	    sleep(1);
	    vmsg("抱歉，目前已有太多 guest 在站上, 請用new註冊。");
	    exit(1);
	}
    }
}
