#include "bbs.h"

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param f_comments fp of the comments, should be in the beginning of the FILE.
 * @return number of comments
 */
unsigned int
fget_n_comments(FILE *f_comments)
{
    unsigned char v;
    unsigned int len;
    read(f_comments, &v, sizeof(unsigned char));
    read(f_comments, &len, sizeof(unsigned int));
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
fget_next_comment(FILE *f_comment, char *buf)
{
    unsigned char len;
    unsigned char len_comments;

    read(f_comments, &len, sizeof(unsigned char));

    lseek(f_comments, COMMENT_OFFSET_LEN_USERNAME, SEEK_CUR);
    read(f_comments, &len_username, sizeof(unsigned char));
    lseek(f_comments, len_username, SEEK_CUR);
    len_comments = len - COMMENT_OFFSET_USERNAME - len_username;
    read(f_comments, buf, len_comments);

    return len_comments;
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param f_comment_reply [description]
 * @return [description]
 */
unsigned int
fget_n_comment_reply(FILE *f_comment_reply)
{
    unsigned char v;
    unsigned int len;
    read(f_comment_reply, &v, sizeof(unsigned char));
    read(f_comment_reply, &len, sizeof(unsigned int));

    return len;
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param f_comment_reply [description]
 * @return [description]
 */
unsigned int
fpeek_next_comment_reply_id(FILE *f_comment_reply)
{
    unsigned short len;
    unsigned int comment_id;

    read(f_comment_reply, &len, sizeof(unsigned short));
    read(f_comment_reply, &comment_id, sizeof(unsigned int));
    lseek(f_comments_reply, -COMMENT_REPLY_OFFSET_TIMESTAMP, SEEK_CUR);

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
fget_next_comment_reply(FILE *f_comment_reply, char **buf)
{
    unsigned short len;
    unsigned int comment_id;
    unsigned long long the_timestamp;
    unsigned short len_buf;

    read(f_comment_reply, &len, sizeof(unsigned short));
    lseek(f_comment_reply, COMMENT_REPLY_OFFSET_REPLY - sizeof(unsigned short), SEEK_CUR);
    len_buf = len - COMMENT_REPLY_OFFSET_REPLY;
    *buf = malloc(len_buf);
    read(f_comment_reply, *buf, len_buf);
    return len_buf;
}
