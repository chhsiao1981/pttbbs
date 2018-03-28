/* $Id$ */
#ifndef PTTDB_CONST_H
#define PTTDB_CONST_H

#include "pttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEN_COMMENT_HEADER 15
#define LEN_COMMENT_PREFIX_COLOR 8

#define LEN_COMMENT_DATETIME_IN_LINE 13

#define COMMENT_CROSS_HEADER "�� " ANSI_COLOR(1;32)
#define LEN_COMMENT_CROSS_HEADER 10 
#define LEN_COMMENT_CROSS_PREFIX_COLOR 7
#define COMMENT_CROSS_PREFIX ":�����"
#define LEN_COMMENT_CROSS_PREFIX 7
#define COMMENT_CROSS_HIDDEN_BOARD "�Y���άݪO"

#define LEN_COMMENT_RESET_INFIX 4
#define COMMENT_RESET_INFIX " �� "
#define LEN_COMMENT_RESET_POSTFIX 13
#define COMMENT_RESET_POSTFIX " �N���˭��k�s"    

#define LEN_LINE_EDIT_PREFIX 9
#define LINE_EDIT_PREFIX "�� �s��: "

#define LEN_STR_POST 5

#define TITLE_PREFIX "���D:"
#define LEN_TITLE_PREFIX 5

#define TIME_PREFIX "�ɶ�:"
#define LEN_TIME_PREFIX 5
#define LEN_DAY_IN_WEEK_STRING 3
#define LEN_TIME_STRING 24

#define WEBLINK_POSTFIX "html"
#define LEN_WEBLINK_POSTFIX 4
#define LEN_AID_POSTFIX 3
#define LEN_AID_INFIX 1
#define LEN_AID_TIMESTAMP 10

#define LEGACY_ORIGIN_IP "�� From:"
#define LEN_LEGACY_ORIGIN_IP 8

#define ORIGIN_PREFIX "�� �o�H��:"
#define LEN_ORIGIN_PREFIX 10

#define MIN_CREATE_TIMESTAMP 810000000

#ifdef __cplusplus
}
#endif

#endif /* PTTDB_CONST_H */