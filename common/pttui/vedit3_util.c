#include "cmpttui/vedt3_util.h"


Err
vedit3_n2ansi(int nx, VEdit3Buffer *line, int *p_ansix)
{
    int ansix = 0;
    char *tmp = line->buf;
    char *nxp = tmp;
    char   ch;

    tmp = nxp = line->buf;
    nxp += nx;

    while (*tmp) {
        if (*tmp == KEY_ESC) {
            while ((ch = *tmp) && !isalpha((int)ch))
                tmp++;
            if (ch)
                tmp++;
            continue;
        }
        if (tmp >= nxp)
            break;
        tmp++;
        ansix++;
    }

    *p_ansix = ansix;
    return S_OK;
}

Err
vedit3_ansi2n(int ansix, VEdit3Buffer *line, int *nx)
{
    char *data = line->buf;
    char *tmp = data;
    char ch = 0;

    while (*tmp) {
        if (*tmp == KEY_ESC) {
            while ((ch = *tmp) && !isalpha((int)ch))
                tmp++;
            if (ch)
                tmp++;
            continue;
        }
        if (ansix <= 0)
            break;
        tmp++;
        ansix--;
    }

    *nx = tmp - data;

    return S_OK;
}
