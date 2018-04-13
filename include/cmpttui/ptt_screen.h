/* $Id$ */
#ifndef PTT_SCREEN_H
#define PTT_SCREEN_H

#include "ptterr.h"
#include "cmpttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TEXTLINE_SIZE 1024

enum LineType {
    LINE_TYPE_MAIN,
    LINE_TYPE_COMMENT,
    LINE_TYPE_COMMENT_REPLY,
    LINE_TYPE_OTHER,
};

typedef struct Textline {
    struct Textline *prev;
    struct Textline *next;

#ifdef DEBUG
    short           mlength;
#endif

    enum LineType   line_type;
    UUID            the_id;
    int             block_idx;
    int             block_offset;
    short           len;

    char            buf[MAX_TEXTLINE_SIZE];

} Textline;   

typedef struct PttScreen {
    Textline *top_line;    
    int n_line;
} PttScreen;

extern PttScreen PTT_SCREEN;

#ifdef __cplusplus
}
#endif

#endif /* PTT_SCREEN_H */

