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
    int fo_main, fo_comments, fo_comment_reply;
    int fi = open(fpath, O_RDONLY);
    int offset_origin = migrate_1to3_get_offset_origin(fi);
    int offset_comments = migrate_1to3_get_offset_comments_from_origin(fi, offset_origin, offset_comments);
    int state = MIGRATE_STATE_INIT;

    char buf[MIGRATE_MERGE_BUF_SIZE];
    char line[MIGRATE_MERGE_BUF_SIZE];

    int bytes;

    char *p_buf = NULL;
    int current_buf_offset = 0;

    int line_offset = 0;
    int bytes_in_line = 0;

    char *p_line = line;
    int bytes_in_new_line = 0;

    // main
    fo_main = OpenCreate(fpath_main, O_WRONLY | O_TRUNC);
    if (fo_main < 0) {
        close(fi);
        return -1;
    }

    lseek(fi, 0, SEEK_SET);
    while (offset_comments > 0 && (bytes = read(fi, buf, sizeof(buf))) > 0) {
        n -= bytes;
        if (n < 0) bytes += n;
        write(fo_main, buf, bytes);
    }

    close(fo_main);

    // fi is expected to be in the end of the main.

    // comment and comment-reply
    fo_comments = OpenCreate(fpath_comments, O_WRONLY | O_TRUNC);
    if (fo_comments < 0) {
        close(fi);
        return -1;
    }
    fo_comment_reply = OpenCreate(fpath_comment_reply, O_WRONLY | O_TRUNC);
    if (fo_comment_reply < 0) {
        close(fo_comments);
        close(fi);
        return -1;
    }

    /*****
     * Definition of the variables:
     *     bytes: total-bytes read from the fi.
     *
     *     p_buf: pointer of the buf
     *     current_buf_offset: the offset of the p_buf in the buf
     *
     *     line: the complete line.
     *     line_offset: offset of the line[0] in fi.
     *     bytes_in_line: length of the line.
     *
     *     p_line: pointer of the line.
     *     bytes_in_new_line: retreived new bytes to the line from migrate_1to3_get_line.
     *
     * Process:
     *     each for-loop correctly get a complete line. XXX assuming each line does not exceed MIGRATE_MERGE_BUF_SIZE
     *
     *
     *****/
    while (bytes = read(fi, buf, sizeof(buf)) > 0) {
        for (current_buf_offset = 0, p_buf = buf; current_buf_offset < bytes; p_buf += bytes_in_new_line, current_buf_offset += bytes_in_new_line) {

            error_code = migrate_1to3_get_line(p_buf, current_buf_offset, bytes, p_line, bytes_in_line, &bytes_in_new_line);
            if (error_code) {
                p_line += bytes_in_new_line;
                bytes_in_line += bytes_in_new_line;
                break;
            }
            bytes_in_line += bytes_in_new_line;

            // MAIN-OP
            state = migrate_1to3_op_by_state(state, line, bytes_in_line, fo_comments, fo_comment_reply);

            // reset line
            line_offset += bytes_in_line;

            p_line = line;
            bytes_in_line = 0;
        }
    }
    // last line
    if (bytes_in_line) state = migrate_1to3_op_by_state(state, line, bytes_in_line, fo_comments, fo_comment_reply);

    close(fo_comments);
    close(fo_comment_reply);
    close(fi);

    return 0;
}

int
migrate_1to3_get_offset_origin(int fd)
{
    char buf[MIGRATE_MERGE_BUF_SIZE];
    char line[MIGRATE_MERGE_BUF_SIZE];

    int bytes;

    char *p_buf = NULL;
    int current_buf_offset = 0;

    int line_offset = 0;
    int bytes_in_line = 0;

    char *p_line = line;
    int bytes_in_new_line = 0;

    while (bytes = read(fd, buf, sizeof(buf)) > 0) {
        for (current_buf_offset = 0, p_buf = buf; current_buf_offset < bytes; p_buf += bytes_in_new_line, current_buf_offset += bytes_in_new_line) {

            error_code = migrate_1to3_get_line(p_buf, current_buf_offset, bytes, p_line, bytes_in_line, &bytes_in_new_line);
            if (error_code) {
                p_line += bytes_in_new_line;
                bytes_in_line += bytes_in_new_line;
                break;
            }
            bytes_in_line += bytes_in_new_line;

            // MAIN-OP
            if (!strncmp(line, MIGRATE_HEADER_ORIGIN, LEN_MIGRATE_HEADER_ORIGIN)) current_offset = line_offset;

            // reset line
            line_offset += bytes_in_line;

            p_line = line;
            bytes_in_line = 0;
        }
    }
    // last line
    if (bytes_in_line) {
        if (!strncmp(line, MIGRATE_HEADER_ORIGIN, LEN_MIGRATE_HEADER_ORIGIN)) current_offset = line_offset;
    }

    return current_offset;
}

int
migrate_1to3_get_offset_comments_from_origin(int fd, int offset_origin)
{
    char buf[MIGRATE_MERGE_BUF_SIZE];
    char line[MIGRATE_MERGE_BUF_SIZE];

    int bytes;

    char *p_buf = NULL;
    int current_buf_offset = 0;

    int line_offset = 0;
    int bytes_in_line = 0;

    char *p_line = line;
    int bytes_in_new_line = 0;

    // start with origin
    lseek(fd, offset_origin, SEEK_SET);

    while (bytes = read(fd, buf, sizeof(buf)) > 0) {
        for (current_buf_offset = 0, p_buf = buf; current_buf_offset < bytes; p_buf += bytes_in_new_line, current_buf_offset += bytes_in_new_line) {

            error_code = migrate_1to3_get_line(p_buf, current_buf_offset, bytes, p_line, bytes_in_line, &bytes_in_new_line);
            if (error_code) {
                p_line += bytes_in_new_line;
                bytes_in_line += bytes_in_new_line;
                break;
            }
            bytes_in_line += bytes_in_new_line;

            if (!bytes_in_line) break;

            // MAIN-OP
            if (migrate_1to3_is_recommend_line(line)) return line_offset;
            if (migrate_1to3_is_boo_line(line)) return line_offset;
            if (migrate_1to3_is_comment_line(line)) return line_offset;
            if (migrate_1to3_is_forward_line(line)) return line_offset;

            // reset line
            line_offset += bytes_in_line;

            p_line = line;
            bytes_in_line = 0;
        }
    }
    // last line
    if (bytes_in_line) {
        if (migrate_1to3_is_recommend_line(line)) return line_offset;
        if (migrate_1to3_is_boo_line(line)) return line_offset;
        if (migrate_1to3_is_comment_line(line)) return line_offset;
        if (migrate_1to3_is_forward_line(line)) return line_offset;
    }

    return -1;
}

/**
 * @brief Try to get a line (ending with \r\n) from buffer.
 * @details [long description]
 *
 * @param p_buf Starting buffer.
 * @param current_buf_offset Current offset of the p_buf in the whole buffers.
 * @param bytes_buf Total bytes of the buffer.
 * @param p_line Starting point of the line.
 * @param bytes_in_line Offset of the line.
 * @param bytes_in_new_line To be obtained bytes in new extracted line.
 * @return Error state: 0: ok. -1: not complete
 */
int
migrate_1to3_get_line(char *p_buf, int current_buf_offset, int bytes_buf, char *p_line, int offset_line, int *bytes_in_new_line)
{
    // check bytes in line and in buf.
    if (bytes_in_line && p_line[-1] == '\r' && p_buf[0] == '\n') {
        *p_line = '\n';
        *bytes_in_new_line = 1;
        return 0;
    }

    // check bytes in buf.
    for (int i = current_buf_offset, i < bytes_buf - 1; i++) {
        if (*p_buf == '\r' && *(p_buf + 1) == '\n') {
            *p_line = '\r';
            *(p_line + 1) = '\n';
            *bytes_in_new_line = i - current_buf_offset + 1 + 1;

            return 0;
        }

        *p_line++ = *p_buf++;
    }

    // last char
    *p_line++ = *p_buf++;
    *bytes_in_new_line = bytes_buf - current_buf_offset;
    return -1;
}

int
migrate_1to3_is_recommend_line(char *line, int len_line)
{
    return 0;
}

int
migrate_1to3_is_boo_line(char *line, int len_line)
{
    return 0;
}

int
migrate_1to3_is_comment_line(char *line, int len_line)
{
    return 0;
}

int
migrate_1to3_is_forward_line(char *line, int len_line)
{
    return 0;
}

int
migrate_1to3_op_by_state(int state, char *line, int fo_comments, int fo_comment_reply)
{
    return 0;
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
