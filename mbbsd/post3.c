#include "bbs.h"

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param f_comments fp of the comments, should be in the beginning of the FILE.
 * @return number of comments
 */
int
fget_n_comments(int fd_comments)
{
    unsigned char v;
    unsigned int len;
    read(fd_comments, &v, sizeof(unsigned char));
    read(fd_comments, &len, sizeof(unsigned int));
    return len;
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param f_comment fp of the comments, should be in the beginning of the next comment (length)
 * @param buf buffer to set with comment-content
 * 
 * @return length of the buffer. (\r\n not included)
 */
unsigned char
fget_next_comment(int fd_comments, char *buf)
{
    unsigned char len;
    unsigned char len_username;
    unsigned char len_comments;

    read(fd_comments, &len, sizeof(unsigned char));

    lseek(fd_comments, COMMENT_OFFSET_LEN_USERNAME, SEEK_CUR);
    read(fd_comments, &len_username, sizeof(unsigned char));
    lseek(fd_comments, len_username, SEEK_CUR);
    len_comments = len - COMMENT_OFFSET_USERNAME - len_username;
    read(fd_comments, buf, len_comments);

    return len_comments;
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param f_comment_reply [description]
 * @return [description]
 */
int
fget_n_comment_reply(int fd_comment_reply)
{
    unsigned char v;
    unsigned int len;
    read(fd_comment_reply, &v, sizeof(unsigned char));
    read(fd_comment_reply, &len, sizeof(unsigned int));

    return len;
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param f_comment_reply [description]
 * @return [description]
 */
int
fpeek_next_comment_reply_id(int fd_comment_reply)
{
    unsigned short len;
    unsigned int comment_id;

    read(fd_comment_reply, &len, sizeof(unsigned short));
    read(fd_comment_reply, &comment_id, sizeof(unsigned int));
    lseek(fd_comment_reply, -COMMENT_REPLY_OFFSET_TIMESTAMP, SEEK_CUR);

    return comment_id;
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param f_comment_reply [description]
 * @param buf * XXX malloc in the function, require free outside the function.
 * 
 * @return [description]
 */
unsigned short
fget_next_comment_reply(int fd_comment_reply, char **buf)
{
    unsigned short len;
    unsigned short len_buf;

    read(fd_comment_reply, &len, sizeof(unsigned short));
    lseek(fd_comment_reply, COMMENT_REPLY_OFFSET_REPLY - sizeof(unsigned short), SEEK_CUR);
    len_buf = len - COMMENT_REPLY_OFFSET_REPLY;
    *buf = malloc(len_buf);
    read(fd_comment_reply, *buf, len_buf);
    return len_buf;
}
