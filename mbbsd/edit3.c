/**
 * edit3.c, 用來提供 bbs 上的 Edit 裡的文字編輯器.
 * 在真正的 edit 之前. 會先有一個選 block 的機制.
 * 
 */
#include "bbs.h"

/**
 * @brief Migrate from single file to main-comments-reply files, and do edit in Edit mode
 * @details [用來]
 * 
 * @param fpath file-path of the tmp-file for editing
 * @param saveheader [description]
 * @param title title
 * @param flags edit-flag (EDITFLAG_ALLOWTITLE, solve-ed-flag-by-board)
 */
int
migrate_vedit3(const char *fpath, int saveheader, char save_title[STRLEN], int flags)
{
    char fpath_main[PATHLEN];
    char fpath_comments[PATHLEN];
    char fpath_comment_reply[PATHLEN];
    int len_fpath = strlen(fpath);

    // setup fpath
    strncpy(fpath_main, fpath, len_fpath);
    strncpy(fpath_comments, fpath, len_fpath);
    strncpy(fpath_comment_reply, fpath, len_fpath);

    strcpy(fpath_main + len_fpath, ".main");
    strcpy(fpath_comments + len_fpath, ".comments");
    strcpy(fpath_comment_reply + len_fpath, ".reply");

    // migrate
    Migrate1To3(fpath, fpath_main, fpath_comments, fpath_comment_reply);

    // vedit3
    vedit3(fpath_main, fpath_comments, fpath_comment_reply, saveheader, title, flags);

    // merge
    Merge3To1(fpath_main, fpath_comments, fpath_comment_reply, fpath);
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param fpath_main [description]
 * @param fpath_comments [description]
 * @param fpath_comment_reply [description]
 * @param saveheader [description]
 * @param save_title [description]
 * @param flags [description]
 * @return [description]
 */
int 
vedit3(const char *fpath_main, const char *fpath_comments, const char *fpath_comment_reply, int saveheader, char save_title[STRLEN], int flags)
{

}
