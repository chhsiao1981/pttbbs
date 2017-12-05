#include "bbs.h"

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param fpath [description]
 * @param fpath_main [description]
 * @param fpath_comments [description]
 * @param fpath_comment_reply [description]
 */
int
migrate_1to3(const char *fpath, const char *fpath_main, const char *fpath_comments, const char *fpath_comment_reply)
{

}

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param fpath_main [description]
 * @param fpath_comments [description]
 * @param fpath_comment_reply [description]
 */
int
merge_3to1(const char *fpath_main, const char *fpath_comments, const char *fpath_comment_reply, const char *fpath)
{
    int fi, fi2, fi3, fo, bytes;
    unsigned int n_comments;
    unsigned char comment_bytes;

    unsigned int n_comment_reply;
    int next_comment_reply_id;
    unsigned short comment_reply_bytes;
    char *comment_reply;
    unsigned int iterated_comment_reply = 0;

    char buf[MIGRATE_MERGE_BUF_SIZE];

    // open fo
    fo = OpenCreate(fpath, O_WRONLY | O_TRUNC)
         if (fo < 0) return -1;

    // copy fpath_main to fpath
    fi = open(fpath_main, O_RDONLY);
    if (fi < 0) {
        close(fo);
        return -1;
    }

    while ((bytes = read(fi, buf, sizeo(buf))) > 0) write(fo, buf, bytes);

    close(fi);

    // comments and comment_reply
    n_comments = fi2 < 0 ? 0 : fget_n_comments(fi2);
    n_comment_reply = fi3 < 0 ? 0 : fget_n_comment_reply(fi3);

    next_comment_reply_id = n_comment_reply ? fpeek_next_comment_reply_id(fi3) : -1;

    for (int i = 0; i < n_comments; i++) {
        comment_bytes = fget_next_comment(fi2, buf);
        write(fo, buf, comment_bytes);

        if (i == next_comment_reply_id) {
            comment_reply_bytes = fget_next_comment_reply(fi3, &comment_reply);
            write(fo, comment_reply, comment_reply_bytes);
            free(comment_reply);
            iterated_comment_reply++;

            if (iterated_comment_reply < n_comment_reply) {
                next_comment_reply_id = fpeek_next_comment_reply_id(fi3);
            }
        }
    }

    close(fi2);
    close(fi3);
    close(fo);

    return 0;
}
