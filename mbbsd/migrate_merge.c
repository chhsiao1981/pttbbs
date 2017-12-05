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
    int fo, fo2, fo3;
    int fi = open(fpath, O_RDONLY);
    int offset_origin = migrate_1to3_get_offset_origin(fi);
    int offset_comments = migrate_1to3_get_offset_comments_from_origin(fi, offset_origin, offset_comments);
    int state = MIGRATE_STATE_INIT;
    int current_buf_offset = 0;
    int bytes;
    int bytes_in_line = 0;
    int line_offset = 0;

    char buf[MIGRATE_MERGE_BUF_SIZE];
    char *p_buf = NULL;

    // main
    fo = OpenCreate(fpath_main, O_WRONLY | O_TRUNC);
    if (fo < 0) {
        close(fi);
        return -1;
    }

    lseek(fi, 0, SEEK_SET);
    while (offset_comments > 0 && (bytes = read(fi, buf, sizeof(buf))) > 0) {
        n -= bytes;
        if (n < 0) bytes += n;
        write(fo, buf, bytes);
    }

    close(fo);

    // fi is expected to be in the end of the main.

    // comment and comment-reply
    fo2 = OpenCreate(fpath_comments, O_WRONLY | O_TRUNC);
    if (fo2 < 0) {
        close(fi);
        return -1;
    }
    fo3 = OpenCreate(fpath_comment_reply, O_WRONLY | O_TRUNC);
    if (fo3 < 0) {
        close(fo2);
        close(fi);
        return -1;
    }

    while (bytes = read(fi, buf, sizeof(buf)) > 0) {
        for (current_buf_offset = 0, p_buf = buf; current_buf_offset < bytes; p_buf += bytes_in_line, current_buf_offset += bytes_in_line, p_line = line) {

            error_code = migrate_1to3_get_line(p_buf, current_buf_offset, p_line, &bytes_in_line);
            if (error_code) {
                break;
            }

            state = migrate_1to3_op_by_state(state, line, fo2, fo3);
        }
    }
    // last line
    if (line_offset) state = migrate_1to3_op_by_state(state, line, fo2, fo3);

    close(fo2);
    close(fo3);
    close(fi);

    return 0;
}

int
migrate_1to3_get_offset_origin(int fd)
{

}

int
migrate_1to3_get_offset_comments_from_origin(int fd, int offset_origin)
{

}

int
migrate_1to3_get_line(char *buf, int current_buf_offset, int bytes_buf, char *line, int *bytes_in_line)
{

}

int
migrate_1to3_op_by_state(int state, char *line, int fo_comments, int fo_comment_reply)
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
    int n_comments;
    unsigned char comment_bytes;

    int n_comment_reply;
    int next_comment_reply_id;
    unsigned short comment_reply_bytes;
    char *comment_reply;
    int iterated_comment_reply = 0;

    char buf[MIGRATE_MERGE_BUF_SIZE];

    // open fo
    fo = OpenCreate(fpath, O_WRONLY | O_TRUNC);
    if (fo < 0) return -1;

    // copy fpath_main to fpath
    fi = open(fpath_main, O_RDONLY);
    if (fi < 0) {
        close(fo);
        return -1;
    }

    while ((bytes = read(fi, buf, sizeof(buf))) > 0) write(fo, buf, bytes);

    close(fi);

    // comments and comment_reply
    fi2 = open(fpath_comments, O_RDONLY);
    fi3 = open(fpath_comment_reply, O_RDONLY);

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

    if (fi2 >= 0) close(fi2);
    if (fi3 >= 0) close(fi3);
    close(fo);

    return 0;
}
