#include "pttdb_const.h"

const enum Karma KARMA_BY_COMMENT_TYPE[] = {
    KARMA_GOOD,
    KARMA_BAD,
    KARMA_ARROW,
    0,
    0,                   // cross
    0,                   // reset
    0,                   // remove
};

const char *COMMENT_TYPE_ATTR[] = {
    "��",
    "�N",
    "��",
    "",
    "��",                 // cross
    "",                  // reset
    "",                  // remove
};

const char *COMMENT_TYPE_ATTR2[] = {
    ANSI_COLOR(1;37),
    ANSI_COLOR(1;31),
    ANSI_COLOR(1;31),
    "",
    "",                  // cross
    "",                  // reset
    "",                  // remove
};
