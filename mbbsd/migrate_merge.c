#include "bbs.h"

/**
 * XXX refer to FormatCommentString in comments.c
 */
char MIGRATE_RECOMMEND_HEADER[] = ANSI_COLOR(1;37) MIGRATE_HEADER_RECOMMEND ANSI_COLOR(33);
char MIGRATE_BOO_HEADER[] = ANSI_COLOR(1;31) MIGRATE_HEADER_BOO ANSI_COLOR(33);
char MIGRATE_COMMENT_HEADER[] = ANSI_COLOR(1;31) MIGRATE_HEADER_COMMENT ANSI_COLOR(33);
int MIGRATE_LEN_COMMENT_HEADER = 15;

char MIGRATE_FORWARD_HEADER0[] = MIGRATE_HEADER_FORWARD0 ANSI_COLOR(1;32); // XXX no green (ANSI_COLOR(32)) in the beginning, system add automatically.
int MIGRATE_LEN_FORWARD_HEADER0 = 10;
char MIGRATE_FORWARD_HEADER1[] = ANSI_COLOR(0;32) MIGRATE_HEADER_FORWARD1;
int MIGRATE_LEN_FORWARD_HEADER1 = 14;
int MIGRATE_LEN_FORWARD_HEADER = 33;
// XXX hack for IDLEN
int MIGRATE_MAX_FORWARD_HEADER = 41;

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
migrate_1to3(const char *fpath, const char *fpath_main, const char *fpath_comments, const char *fpath_comment_reply, const char *fpath_comment_reply_idx)
{
    int fo_main, fo_comments, fo_comment_reply, fo_comment_reply_idx;
    int fi = open(fpath, O_RDONLY);
    int offset_origin = migrate_1to3_get_offset_origin(fi);
    int offset_comments = migrate_1to3_get_offset_comments_from_origin(fi, offset_origin);
    int state = MIGRATE_STATE_INIT;

    char buf[MIGRATE_MERGE_BUF_SIZE];
    char line[MIGRATE_MERGE_BUF_SIZE];

    char reply_buffer[MIGRATE_MERGE_BUF_SIZE];
    int len_reply_buffer = 0;

    int bytes;

    int current_buf_offset = 0;

    int line_offset = 0;
    int bytes_in_line = 0;

    int bytes_in_new_line = 0;
    int n_main = 0;
    int error_code = MIGRATE_S_OK;

    CommentsHeader comments_header = {0, 0, sizeof(unsigned char) + sizeof(int) + sizeof(int)};
    CommentReplyHeader comment_reply_header = {0, 0, sizeof(unsigned char) + sizeof(int) + sizeof(int)};
    CommentReplyIdxHeader comment_reply_idx_header = {0, 0, sizeof(unsigned char) + sizeof(int) + sizeof(int)};

    bzero(reply_buffer, sizeof(reply_buffer));

    // main
    fo_main = OpenCreate(fpath_main, O_WRONLY | O_TRUNC);
    if (fo_main < 0) {
        close(fi);
        return -1;
    }

    lseek(fi, 0, SEEK_SET);
    n_main = offset_comments;
    while (n_main > 0 && (bytes = read(fi, buf, sizeof(buf))) > 0) {
        n_main -= bytes;
        if (n_main < 0) bytes += n_main;
        write(fo_main, buf, bytes);
    }

    close(fo_main);

    // fi is expected to be in the end of the main.
    lseek(fi, offset_comments, SEEK_SET);

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

    fo_comment_reply_idx = OpenCreate(fpath_comment_reply_idx, O_WRONLY | O_TRUNC);
    if (fo_comment_reply_idx < 0) {
        close(fo_comments);
        close(fo_comment_reply);
        close(fi);
        return -1;
    }

    /*****
     * Definition of the variables:
     *     bytes: total-bytes read from the fi.
     *     current_buf_offset: the offset of the p_buf in the buf
     *
     *     line: the complete line.
     *     line_offset: offset of the line[0] in fi.
     *     bytes_in_line: length of the line.
     *
     *     bytes_in_new_line: retreived new bytes to the line from migrate_1to3_get_line.
     *
     * Process:
     *     each for-loop correctly get a complete line. XXX assuming each line does not exceed MIGRATE_MERGE_BUF_SIZE
     *
     *
     *****/
    bzero(line, sizeof(line));
    while ((bytes = read(fi, buf, sizeof(buf))) > 0) {
        for (current_buf_offset = 0; current_buf_offset < bytes; current_buf_offset += bytes_in_new_line) {

            error_code = migrate_1to3_get_line(buf, current_buf_offset, bytes, line, bytes_in_line, &bytes_in_new_line);
            bytes_in_line += bytes_in_new_line;
            if (error_code) {
                break;
            }

            // MAIN-OP
            state = migrate_1to3_op_by_state(state, line, bytes_in_line, reply_buffer, &len_reply_buffer, &comments_header, &comment_reply_header, &comment_reply_idx_header, fo_comments, fo_comment_reply, fo_comment_reply_idx);

            // reset line
            line_offset += bytes_in_line;
            bzero(line, sizeof(char) * bytes_in_line);
            bytes_in_line = 0;
        }
    }
    // last line
    if (bytes_in_line) {
        state = migrate_1to3_op_by_state(state, line, bytes_in_line, reply_buffer, &len_reply_buffer, &comments_header, &comment_reply_header, &comment_reply_idx_header, fo_comments, fo_comment_reply, fo_comment_reply_idx);
    }

    state = MIGRATE_STATE_END;

    state = migrate_1to3_op_by_state(state, line, bytes_in_line, reply_buffer, &len_reply_buffer, &comments_header, &comment_reply_header, &comment_reply_idx_header, fo_comments, fo_comment_reply, fo_comment_reply_idx);

    close(fo_comments);
    close(fo_comment_reply);
    close(fo_comment_reply_idx);
    close(fi);

    return 0;
}

int
migrate_1to3_get_offset_origin(int fd)
{
    char buf[MIGRATE_MERGE_BUF_SIZE];
    char line[MIGRATE_MERGE_BUF_SIZE];

    int bytes;

    int current_buf_offset = 0;

    int line_offset = 0;
    int bytes_in_line = 0;

    int bytes_in_new_line = 0;

    int error_code = MIGRATE_S_OK;
    int current_offset = 0;

    bzero(line, sizeof(line));
    /*****
     * Definition of the variables:,
     *     bytes: total-bytes read from the fi.
     *     current_buf_offset: the offset of the p_buf in the buf
     *
     *     line: the complete line.
     *     line_offset: offset of the line[0] in fi.
     *     bytes_in_line: length of the line.
     *
     *     bytes_in_new_line: retreived new bytes to the line from migrate_1to3_get_line.
     *
     * Process:
     *     each for-loop correctly get a complete line. XXX assuming each line does not exceed MIGRATE_MERGE_BUF_SIZE
     *
     *
     *****/
    while ((bytes = read(fd, buf, sizeof(buf))) > 0) {
        for (current_buf_offset = 0; current_buf_offset < bytes; current_buf_offset += bytes_in_new_line) {

            error_code = migrate_1to3_get_line(buf, current_buf_offset, bytes, line, bytes_in_line, &bytes_in_new_line);
            bytes_in_line += bytes_in_new_line;
            if (error_code) {
                break;
            }

            // MAIN-OP
            if (!strncmp(line, MIGRATE_HEADER_ORIGIN, LEN_MIGRATE_HEADER_ORIGIN)) current_offset = line_offset;

            printf("line: %sbytes_in_line: %dline_offset: %d current_offset: %d\n", line, bytes_in_line, line_offset, current_offset);

            // reset line
            line_offset += bytes_in_line;
            bzero(line, sizeof(char) * bytes_in_line);
            bytes_in_line = 0;
        }
    }
    // last line
    if (bytes_in_line) {
        if (!strncmp(line, MIGRATE_HEADER_ORIGIN, LEN_MIGRATE_HEADER_ORIGIN)) current_offset = line_offset;
        printf("line: %sline_offset: %d current_offset: %d\n", line, line_offset, current_offset);
    }

    return current_offset;
}

int
migrate_1to3_get_offset_comments_from_origin(int fd, int offset_origin)
{
    char buf[MIGRATE_MERGE_BUF_SIZE];
    char line[MIGRATE_MERGE_BUF_SIZE];

    int bytes;

    int current_buf_offset = 0;

    int line_offset = offset_origin;
    int bytes_in_line = 0;

    int bytes_in_new_line = 0;

    int error_code = MIGRATE_S_OK;

    //init line
    bzero(line, sizeof(line));

    // start with origin
    lseek(fd, offset_origin, SEEK_SET);

    /*****
     * Definition of the variables:
     *     bytes: total-bytes read from the fi.
     *     current_buf_offset: the offset of the p_buf in the buf
     *
     *     line: the complete line.
     *     line_offset: offset of the line[0] in fi.
     *     bytes_in_line: length of the line.
     *
     *     bytes_in_new_line: retreived new bytes to the line from migrate_1to3_get_line.
     *
     * Process:
     *     each for-loop correctly get a complete line. XXX assuming each line does not exceed MIGRATE_MERGE_BUF_SIZE
     *
     *
     *****/
    while ((bytes = read(fd, buf, sizeof(buf))) > 0) {
        for (current_buf_offset = 0; current_buf_offset < bytes; current_buf_offset += bytes_in_new_line) {

            error_code = migrate_1to3_get_line(buf, current_buf_offset, bytes, line, bytes_in_line, &bytes_in_new_line);
            bytes_in_line += bytes_in_new_line;
            if (error_code) {
                printf("not reading end of line: line: %s\n", line);
                break;
            }

            printf("line: %sline_offset: %d\n", line, line_offset);

            // MAIN-OP
            if (migrate_1to3_is_comments_line(line, bytes_in_line)) return line_offset;

            // reset line
            line_offset += bytes_in_line;
            bzero(line, sizeof(char) * bytes_in_line);
            bytes_in_line = 0;
        }
    }
    // last line
    if (bytes_in_line) {
        printf("line: %sline_offset: %d\n", line, line_offset);

        if (migrate_1to3_is_comments_line(line, bytes_in_line)) return line_offset;
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
 * @return Error
 */
int
migrate_1to3_get_line(char *p_buf, int current_buf_offset, int bytes_buf, char *p_line, int offset_line, int *bytes_in_new_line)
{
    int i;

    // check the end of buf
    if (current_buf_offset >= bytes_buf) {
        *bytes_in_new_line = 0;

        return MIGRATE_S_ERR;
    }

    // init p_buf offset
    p_buf += current_buf_offset;
    p_line += offset_line;

    // check bytes in line and in buf.
    if (offset_line && p_line[-1] == '\r' && p_buf[0] == '\n') {
        *p_line = '\n';
        *bytes_in_new_line = 1;

        return MIGRATE_S_OK;
    }

    // check bytes in buf.
    for (i = current_buf_offset; i < bytes_buf - 1; i++) {
        if (*p_buf == '\r' && *(p_buf + 1) == '\n') {
            *p_line = '\r';
            *(p_line + 1) = '\n';
            *bytes_in_new_line = i - current_buf_offset + 1 + 1;

            return MIGRATE_S_OK;
        }

        *p_line++ = *p_buf++;
    }

    // last char
    *p_line++ = *p_buf++;
    *bytes_in_new_line = bytes_buf - current_buf_offset;

    return MIGRATE_S_ERR;
}

int
migrate_1to3_is_comments_line(char *line, int len_line)
{
    return migrate_1to3_is_recommend_line(line, len_line) || migrate_1to3_is_boo_line(line, len_line) || migrate_1to3_is_comment_line(line, len_line) || migrate_1to3_is_forward_line(line, len_line);
}

int
migrate_1to3_is_recommend_line(char *line, int len_line)
{
    if (len_line < MIGRATE_LEN_COMMENT_HEADER) return NA;

    return !strncmp(line, MIGRATE_RECOMMEND_HEADER, MIGRATE_LEN_COMMENT_HEADER);
}

int
migrate_1to3_is_boo_line(char *line, int len_line)
{
    if (len_line < MIGRATE_LEN_COMMENT_HEADER) return NA;

    return !strncmp(line, MIGRATE_BOO_HEADER, MIGRATE_LEN_COMMENT_HEADER);
}

int
migrate_1to3_is_comment_line(char *line, int len_line)
{
    if (len_line < MIGRATE_LEN_COMMENT_HEADER) return NA;

    return !strncmp(line, MIGRATE_COMMENT_HEADER, MIGRATE_LEN_COMMENT_HEADER);
}

/**
 * @brief
 * @details refer to bbs.c line: 2258 (snprintf in cross_post)
 *
 * @param line [description]
 * @param len_line [description]
 */
int
migrate_1to3_is_forward_line(char *line, int len_line)
{
    int i;
    int loop_i;
    char *p_line;

    if (len_line < MIGRATE_LEN_FORWARD_HEADER) return NA;
    if (strncmp(line, MIGRATE_FORWARD_HEADER0, MIGRATE_LEN_FORWARD_HEADER0)) return NA;

    p_line = line + MIGRATE_LEN_FORWARD_HEADER0;
    loop_i = len_line < MIGRATE_MAX_FORWARD_HEADER ? len_line : MIGRATE_MAX_FORWARD_HEADER;
    for (i = MIGRATE_LEN_FORWARD_HEADER0, p_line = line + MIGRATE_LEN_FORWARD_HEADER0; i < loop_i; i++, p_line++) {
        if (!migrate_1to3_is_username_char(*p_line))
            return !strncmp(p_line, MIGRATE_FORWARD_HEADER1, MIGRATE_LEN_FORWARD_HEADER1);
    }
    return NA;
}

int
migrate_1to3_is_edit_line(char *line, int len_line)
{
    return NA;
}

int
migrate_1to3_is_forward_from_mailbox(char *line, int len_line)
{
    return NA;
}

int
migrate_1to3_is_username_char(char ch) {
    return isalnum(ch);
}

/**
 * @brief determine the state of line and do the corresponding op.
 * @details determine the state of the line based on current state and the line.
 *          STATE_INIT
 *          STATE_COMMENT
 *          STATE_REPLY
 *          STATE_END
 *          STATE_ERROR
 *          
 *          if state is in STATE_COMMENT:
 *              if line is comments:
 *                  1. set line to new comments
 *                  2. state is in STATE_COMMENT
 *              else
 *                  1. set line into reply-buffer
 *                  2. state is in STATE_REPLY
 *          if state is in STATE_IS_REPLY:
 *              if line is comments:
 *                  1. set reply-buffer to the corresponding comment.
 *                  2. set line to new comments.
 *                  3. state is in STATE_COMMENT
 *              else:
 *                  1. set line to reply-buffer
 *                  2. state is in STATE_REPLY
 *          if state is STATE_INIT:
 *              if line is comments:
 *                  1. set line to new comments
 *                  2. state is in STATE_COMMENT
 *              else:
 *                  return error    
 *          if state is STATE_END:
 *              if reply-buffer:
 *                  set reply-buffer to the corresponding comment.
 *              set fo_comments_header and fo_comment_reply_header.
 *          
 * 
 * @param state [description]
 * @param line [description]
 * @param len_line [description]
 * @param fo_comments [description]
 * @param fo_comment_reply [description]
 */
int
migrate_1to3_op_by_state(int state, char *line, int len_line, char *reply_buffer, int *len_reply_buffer, CommentsHeader *p_comments_header, CommentReplyHeader *p_comment_reply_header, CommentReplyIdxHeader *p_comment_reply_idx_header, int fo_comments, int fo_comment_reply, int fo_comment_reply_idx)
{    
    int new_state;
    switch(state) {
    case MIGRATE_STATE_INIT:
        new_state = migrate_1to3_op_by_state_init(line, len_line, reply_buffer, len_reply_buffer, p_comments_header, p_comment_reply_header, p_comment_reply_idx_header, fo_comments, fo_comment_reply, fo_comment_reply_idx);
        break;
    case MIGRATE_STATE_COMMENT:
        new_state = migrate_1to3_op_by_state_comment(line, len_line, reply_buffer, len_reply_buffer, p_comments_header, p_comment_reply_header, p_comment_reply_idx_header, fo_comments, fo_comment_reply, fo_comment_reply_idx);
        break;
    case MIGRATE_STATE_REPLY:
        new_state = migrate_1to3_op_by_state_reply(line, len_line, reply_buffer, len_reply_buffer, p_comments_header, p_comment_reply_header, p_comment_reply_idx_header, fo_comments, fo_comment_reply, fo_comment_reply_idx);
        break;
    case MIGRATE_STATE_END:
        new_state = migrate_1to3_op_by_state_end(line, len_line, reply_buffer, len_reply_buffer, p_comments_header, p_comment_reply_header, p_comment_reply_idx_header, fo_comments, fo_comment_reply, fo_comment_reply_idx);
        break;
    }
    printf("line: %slen_line: %dstate: %d new_state: %d reply_buffer: %s len_reply_buffer: %d n-comments: %d comments-size: %d, n-comment-reply: %d comment-reply-size: %d n-comment-reply-idx: %d comment-reply-idx-size: %d\n", line, len_line, state, new_state, reply_buffer, *len_reply_buffer, p_comments_header->n_comments, p_comments_header->the_size, p_comment_reply_header->n_comment_reply, p_comment_reply_header->the_size, p_comment_reply_idx_header->n_comment_reply, p_comment_reply_idx_header->the_size);
    return new_state;
}


int
migrate_1to3_op_by_state_init(char *line, int len_line, char *reply_buffer, int *len_reply_buffer, CommentsHeader *p_comments_header, CommentReplyHeader *p_comment_reply_header, CommentReplyIdxHeader *p_comment_reply_idx_header, int fo_comments, int fo_comment_reply, int fo_comment_reply_idx)
{
    if(migrate_1to3_is_comments_line(line, len_line)) {
        migrate_1to3_set_new_comment(line, len_line, p_comments_header, fo_comments);
        return MIGRATE_STATE_COMMENT;
    }
    else
        return MIGRATE_S_ERR;
}

int
migrate_1to3_op_by_state_comment(char *line, int len_line, char *reply_buffer, int *len_reply_buffer, CommentsHeader *p_comments_header, CommentReplyHeader *p_comment_reply_header, CommentReplyIdxHeader *p_comment_reply_idx_header, int fo_comments, int fo_comment_reply, int fo_comment_reply_idx)
{
    if(migrate_1to3_is_comments_line(line, len_line)) {
        migrate_1to3_set_new_comment(line, len_line, p_comments_header, fo_comments);
        return MIGRATE_STATE_COMMENT;
    }
    else {
        migrate_1to3_set_reply_buffer(line, len_line, reply_buffer, len_reply_buffer);
        return MIGRATE_STATE_REPLY;
    }
}

int
migrate_1to3_op_by_state_reply(char *line, int len_line, char *reply_buffer, int *len_reply_buffer, CommentsHeader *p_comments_header, CommentReplyHeader *p_comment_reply_header, CommentReplyIdxHeader *p_comment_reply_idx_header, int fo_comments, int fo_comment_reply, int fo_comment_reply_idx)
{
    if(migrate_1to3_is_comments_line(line, len_line)) {
        migrate_1to3_set_reply_buffer_to_comment_reply(reply_buffer, *len_reply_buffer, p_comments_header, p_comment_reply_header, p_comment_reply_idx_header, fo_comment_reply, fo_comment_reply_idx);
        return MIGRATE_STATE_COMMENT;
    }
    else {
        migrate_1to3_set_reply_buffer(line, len_line, reply_buffer, len_reply_buffer);
        return MIGRATE_STATE_REPLY;
    }
}

int
migrate_1to3_op_by_state_end(char *line, int len_line, char *reply_buffer, int *len_reply_buffer, CommentsHeader *p_comments_header, CommentReplyHeader *p_comment_reply_header, CommentReplyIdxHeader *p_comment_reply_idx_header, int fo_comments, int fo_comment_reply, int fo_comment_reply_idx)
{
    if(*len_reply_buffer) {
        migrate_1to3_set_reply_buffer_to_comment_reply(reply_buffer, *len_reply_buffer, p_comments_header, p_comment_reply_header, p_comment_reply_idx_header, fo_comment_reply, fo_comment_reply_idx);
    }

    return MIGRATE_STATE_END;
}

int
migrate_1to3_set_new_comment(char *line, int len_line, CommentsHeader *p_comments_header, int fo_comments)
{
    // the_size
    int the_size = (int)sizeof(int) + len_line;
    write(fo_comments, &the_size, sizeof(int));

    // the_comments
    write(fo_comments, line, len_line);
    p_comments_header->n_comments++;
    p_comments_header->the_size += len_line;

    return 0;
}

int
migrate_1to3_set_reply_buffer(char *line, int len_line, char *reply_buffer, int *len_reply_buffer)
{
    len_line = len_line + *len_reply_buffer < MIGRATE_MERGE_BUF_SIZE ? len_line : (MIGRATE_MERGE_BUF_SIZE - *len_reply_buffer);
    if(len_line <= 0) return MIGRATE_S_ERR;

    reply_buffer += *len_reply_buffer;
    memcpy(reply_buffer, line, len_line);
    *len_reply_buffer += len_line;

    return 0;
}

int
migrate_1to3_set_reply_buffer_to_comment_reply(char *reply_buffer, int len_reply_buffer, CommentsHeader *p_comments_header, CommentReplyHeader *p_comment_reply_header, CommentReplyIdxHeader *p_comment_reply_idx_header, int fo_comment_reply, int fo_comment_reply_idx)
{
    /**
     * comment-reply-idx
     */
    p_comment_reply_idx_header->n_comment_reply++;
    p_comment_reply_idx_header->the_size += 8;

    // comment-id
    write(fo_comment_reply_idx, &p_comments_header->n_comments, sizeof(int));
    // comment-reply offset
    write(fo_comment_reply_idx, &p_comment_reply_header->the_size, sizeof(int));


    /**
     * comment-reply
     */
    int the_size = (int)sizeof(int) + (int)sizeof(int) + len_reply_buffer;

    // the_size
    write(fo_comment_reply, &the_size, sizeof(int));

    // comment-id
    write(fo_comment_reply, &p_comments_header->n_comments, sizeof(int));

    write(fo_comment_reply, reply_buffer, len_reply_buffer);

    p_comment_reply_header->n_comment_reply++;
    p_comment_reply_header->the_size += the_size;

    bzero(reply_buffer, sizeof(char) * len_reply_buffer);

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
