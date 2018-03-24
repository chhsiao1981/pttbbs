/* $Id$ */
#ifndef PTTDB_CONSNT_H
#define PTTDB_CONST_H

#include "pttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEN_COMMENT_HEADER 15
    
#define COMMENT_FORAWRD_PREFIX ":轉錄至看板 "
#define LEN_COMMENT_FORWARD_PREFIX 12

#define LEN_STR_POST 5

#define TITLE_PREFIX "標題:"
#define LEN_TITLE_PREFIX 5

#define TIME_PREFIX "時間:"
#define LEN_TIME_PREFIX 5
#define LEN_DAY_IN_WEEK_STRING 3
#define LEN_TIME_STRING 24

#define WEBLINK_POSTFIX "html"
#define LEN_WEBLINK_POSTFIX 4
#define LEN_AID_POSTFIX 3
#define LEN_AID_INFIX 1
#define LEN_AID_TIMESTAMP 10

#define LEGACY_ORIGIN_IP "◆ From:"
#define LEN_LEGACY_ORIGIN_IP 8

#define ORIGIN_PREFIX "※ 發信站:"
#define LEN_ORIGIN_PREFIX 10

#define MIN_CREATE_TIMESTAMP 810000000

#ifdef __cplusplus
}
#endif

#endif /* PTTDB_H */