#include "cmpttui/pttui_util.h"

Err
pttui_next_non_space_char(char *buf, int len, char **p_buf)
{
    char *tmp_buf = buf;
    int i = 0;
    for(i = 0; i < len && *tmp_buf == ' '; i++, tmp_buf++);
    if(i == len) {
        *p_buf = NULL;
        return S_OK;
    }

    *p_buf = tmp_buf;

    return S_OK;
}

Err
pttui_first_word(char *buf, int len, int *len_word)
{
    char *tmp_buf = buf;
    int i = 0;
    for(i = 0; i < len && *tmp_buf != ' '; i++, tmp_buf++);

    *len_word = i;

    return S_OK;
}

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

/**
 * @brief [brief description]
 * @details ref: mchar_len in edit.c
 *          return int directly because of aligning with strlen
 * 
 * @param char [description]
 * @return [description]
 */
int
pttui_mchar_len(unsigned char *str)
{
  return ((str[0] != '\0' && str[1] != '\0' && IS_BIG5(str[0], str[1])) ?
            2 :
            1);
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param str [description]
 * @param pos [description]
 * @param dir [description]
 * @param new_pos [description]
 */
Err
pttui_fix_cursor(char *str, int pos, enum PttUIFixCursorDir dir, int *new_pos)
{
    int tmp_newpos = 0, w = 0;
    while (*str != '\0' && tmp_newpos < pos) {
        w = pttui_mchar_len((unsigned char *) str);
        str += w;
        tmp_newpos += w;
    }
    if (dir == PTTUI_FIX_CURSOR_DIR_LEFT && tmp_newpos > pos) tmp_newpos -= w;

    *new_pos = tmp_newpos;

    return S_OK;
}

/**
 * @brief [brief description]
 * @details ref: phone_mode_switch in edit.c
 *
 * @param is_phone [description]
 * @param phone_mode [description]
 */
inline Err
pttui_phone_mode_switch(bool *is_phone, int *phone_mode)
{
    *is_phone ^= 1;
    if (*is_phone) {
        if (!*phone_mode) *phone_mode = 2;
    }

    return S_OK;
}

/**
 * @brief [brief description]
 * @details ref: phone_char in edit.c
 *
 * @param c [description]
 * @param phone_mode [description]
 * @param ret [description]
 */
Err
pttui_phone_char(char c, int phone_mode, char **ret)
{
    char *tmp_ret = NULL;
    if (phone_mode > 0 && phone_mode < 20) {
        if (tolower(c) < 'a' ||
                (tolower(c) - 'a') >= (int)strlen(BIG5[phone_mode - 1]) / 2) {
            tmp_ret = NULL;
        }
        else {
            tmp_ret = BIG5[phone_mode - 1] + (tolower(c) - 'a') * 2;
        }
    }
    else if (phone_mode >= 20) {
        if (c == '.') c = '/';

        if (c < '/' || c > '9') {
            tmp_ret = NULL;
        }
        else {
            tmp_ret = table[phone_mode - 20] + (c - '/') * 2;
        }
    }
    else {
        tmp_ret = NULL;
    }

    *ret = tmp_ret;

    return S_OK;
}

inline Err
pttui_phone_mode_filter(char ch, bool is_phone, int *phone_mode, char *ret, bool *is_redraw_everything)
{
    if (!is_phone) {
        *ret = 0;
        return S_OK;
    }

    int orig_phone_mode = *phone_mode;

    switch (ch) {
    case 'z':
    case 'Z':
        if (orig_phone_mode < 20) {
            *phone_mode = 20;
        }
        else {
            *phone_mode = 2;
        }

        *ret = ch;
        return S_OK;
        break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        if (orig_phone_mode < 20) {

            *phone_mode = ch - '0' + 1;
            *is_redraw_everything = true;
            *ret = ch;
            return S_OK;
        }
        break;
    case '-':
        if (orig_phone_mode < 20) {
            *phone_mode = 11;

            *is_redraw_everything = true;
            *ret = ch;
            return S_OK;
        }
        break;
    case '=':
        if (orig_phone_mode < 20) {
            *phone_mode = 12;

            *is_redraw_everything = true;
            *ret = ch;
            return S_OK;
        }
        break;
    case '`':
        if (orig_phone_mode < 20) {
            *phone_mode = 13;

            *is_redraw_everything = true;
            *ret = ch;
            return S_OK;
        }
        break;
    case '/':
        if (orig_phone_mode >= 20) {
            (*phone_mode) += 4;
            if (*phone_mode > 27) *phone_mode -= 8;

            *is_redraw_everything = true;
            *ret = ch;
            return S_OK;
        }
        break;
    case '*':
        if (orig_phone_mode >= 20) {
            (*phone_mode)++;
            if ((*phone_mode - 21) % 4 == 3) *phone_mode -= 4;

            *is_redraw_everything = true;
            *ret = ch;
            return S_OK;
        }
        break;
    default:
        break;
    }

    *ret = 0;
    return S_OK;
}
