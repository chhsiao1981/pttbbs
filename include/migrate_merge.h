#ifndef INCLUDE_MIGRATE_MERGE_H
#define INCLUDE_MIGRATE_MERGE_H

typedef struct {
    unsigned char version;
    int n_comments;
    int the_size;
} CommentsHeader;

typedef struct {
    int the_size;
    char *buf;
    int max_buf_size; // XXX not in the record.
} Comment;

typedef struct {
    unsigned char version;
    int n_comment_reply;
    int the_size;
} CommentReplyHeader;

typedef struct {
    int the_size;
    int comment_id;
    char *buf;
    int max_buf_size; // XXX not in the record.
} CommentReply;

typedef struct {
    unsigned char version;
    int n_comment_reply;
    int the_size;
} CommentReplyIdxHeader;

typedef struct {
    int comment_id;
    int offset_comment_reply;
} CommentReplyIdx;

int migrate_1to3(const char *fpath, const char *fpath_main, const char *fpath_comments, const char *fpath_comment_reply);

int migrate_1to3_get_offset_origin(int fd);
int migrate_1to3_get_offset_comments_from_origin(int fd, int offset_origin);
int migrate_1to3_get_line(char *buf, int current_buf_offset, int bytes_buf, char *line, int offset_line, int *bytes_in_new_line);

int migrate_1to3_is_comments_line(char *line, int len_line);
int migrate_1to3_is_recommend_line(char *line, int len_line);
int migrate_1to3_is_boo_line(char *line, int len_line);
int migrate_1to3_is_comment_line(char *line, int len_line);
int migrate_1to3_is_forward_line(char *line, int len_line);
int migrate_1to3_is_edit_line(char *line, int len_line);
int migrate_1to3_is_forward_from_mailbox(char *line, int len_line);
int migrate_1to3_is_username_char(char ch);

int migrate_1to3_op_by_state(int state, char *line, int len_line, char *reply_buffer, int *len_reply_buffer, CommentsHeader *p_comments_header, CommentReplyHeader *p_comment_reply_header, CommentReplyIdxHeader *p_comment_reply_idx_header, int fo_comments, int fo_comment_reply, int fo_comment_reply_idx);
int migrate_1to3_op_by_state_init(char *line, int len_line, char *reply_buffer, int *len_reply_buffer, CommentsHeader *p_comments_header, CommentReplyHeader *p_comment_reply_header, CommentReplyIdxHeader *p_comment_reply_idx_header, int fo_comments, int fo_comment_reply, int fo_comment_reply_idx);
int migrate_1to3_op_by_state_comment(char *line, int len_line, char *reply_buffer, int *len_reply_buffer, CommentsHeader *p_comments_header, CommentReplyHeader *p_comment_reply_header, CommentReplyIdxHeader *p_comment_reply_idx_header, int fo_comments, int fo_comment_reply, int fo_comment_reply_idx);
int migrate_1to3_op_by_state_reply(char *line, int len_line, char *reply_buffer, int *len_reply_buffer, CommentsHeader *p_comments_header, CommentReplyHeader *p_comment_reply_header, CommentReplyIdxHeader *p_comment_reply_idx_header, int fo_comments, int fo_comment_reply, int fo_comment_reply_idx);
int migrate_1to3_op_by_state_end(char *line, int len_line, char *reply_buffer, int *len_reply_buffer, CommentsHeader *p_comments_header, CommentReplyHeader *p_comment_reply_header, CommentReplyIdxHeader *p_comment_reply_idx_header, int fo_comments, int fo_comment_reply, int fo_comment_reply_idx);

int migrate_1to3_set_new_comment(char *line, int len_line, CommentsHeader *p_comments_header, int fo_comments);
int migrate_1to3_set_reply_buffer(char *line, int len_line, char *reply_buffer, int *len_reply_buffer);
int migrate_1to3_set_reply_buffer_to_comment_reply(char *reply_buffer, int len_reply_buffer, CommentsHeader *p_comments_header, CommentReplyHeader *p_comment_reply_header, CommentReplyIdxHeader *p_comment_reply_idx_header, int fo_comment_reply, int fo_comment_reply_idx);


int merge_3to1(const char *fpath_main, const char *fpath_comments, const char *fpath_comment_reply, const char *fpath);

#endif
