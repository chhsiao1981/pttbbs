#include "cmpttui/pttui_util.h"

Err
pttui_raw_shift_right(char *s, int len)
{
    int i;

    for (i = len - 1; i >= 0; --i) s[i + 1] = s[i];

    return S_OK;
}

Err
pttui_raw_shift_left(char *s, int len)
{
    int i;
    for (i = 0; i < len && s[i] != 0; ++i) s[i] = s[i + 1];

    return S_OK;
}

/**
 * @brief [brief description]
 * @details ref: ansi2n in edit.c
 *
 * @param ansix [description]
 * @param buf [description]
 * @param nx [description]
 */
Err
pttui_ansi2n(int ansix, char *buf, int *nx)
{
    char *tmp = buf;
    char ch;

    while (*tmp) {
        if (*tmp == KEY_ESC) {
            // XXX tmp may be out of range
            while ((ch = *tmp) && !isalpha((int)ch)) tmp++;

            if (ch) tmp++;

            continue;
        }

        if (ansix <= 0) break;

        tmp++;
        ansix--;
    }

    *nx = tmp - buf;

    return S_OK;
}

/**
 * @brief [brief description]
 * @details n2ansi in edit.c
 *
 * @param nx [description]
 * @param buf [description]
 * @param ansix [description]
 */
Err
pttui_n2ansi(int nx, char *buf, int *ansix)
{
    char *tmp = buf;
    char *nxp = buf + nx;
    char ch;

    int tmp_ansix = 0;

    while (*tmp) {
        if (*tmp == KEY_ESC) {
            while ((ch = *tmp) && !isalpha((int)ch)) tmp++;

            if (ch) tmp++;

            continue;
        }

        if (tmp >= nxp) break;

        tmp++;
        tmp_ansix++;
    }

    *ansix = tmp_ansix;

    return S_OK;
}
