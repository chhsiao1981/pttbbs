#include "pttdb_const.h"

const enum Karma KARMA_BY_COMMENT_TYPE[] = {
    KARMA_GOOD,
    KARMA_BAD,
    KARMA_ARROW,
    0,
    0,                   // forward
    0,                   // other
};

const char *COMMENT_TYPE_ATTR[] = {
    "±À",
    "¼N",
    "¡÷",
    "",
    "¡°",
    "",
};

const char *COMMENT_TYPE_ATTR2[] = {
    ANSI_COLOR(1;37),
    ANSI_COLOR(1;31),
    ANSI_COLOR(1;31),
    "",
    "",
    "",
};
