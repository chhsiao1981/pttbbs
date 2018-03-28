#include "pttdb_util.h"

/**********
 * Get line from buf
 **********/

/**
 * @brief Try to get a line (ending with \n) from buffer.
 * @details [long description]
 *
 * @param p_buf Starting point of the buffer.
 * @param offset_buf Current offset of the p_buf in the whole buffer.
 * @param bytes Total bytes of the buffer.
 * @param p_line Starting point of the line.
 * @param offset_line Offset of the line.
 * @param bytes_in_new_line To be obtained bytes in new extracted line.
 * @return Error
 */
Err
get_line_from_buf(char *buf, int offset_buf, int buf_size, char *p_line, int offset_line, int line_size, int *bytes_in_new_line)
{

    // check the end of buf
    if (offset_buf >= buf_size) {
        *bytes_in_new_line = 0;

        return S_ERR;
    }

    // init p_buf offset
    char *p_buf = buf + offset_buf;
    p_line += offset_line;

    // check \n in buf.
    int max_new_lines = line_size - offset_line;
    int iter_bytes = (buf_size - offset_buf <= max_new_lines) ? (buf_size - offset_buf) : max_new_lines;

    //fprintf(stderr, "pttdb_util.get_line_from_buf: buf_size: %d offset_buf: %d max_new_lines: %d iter_bytes: %d\n", buf_size, offset_buf, max_new_lines, iter_bytes);
    for (int i = 0; i < iter_bytes; i++) {
        //fprintf(stderr, "pttdb_util.get_line_from_buf: (%d/%d) p_buf: %d\n", i, iter_bytes, *p_buf);
        if (*p_buf == '\n') {
            *p_line = '\n';
            *bytes_in_new_line = i + 1;
 
            return S_OK;
        }

        *p_line++ = *p_buf++;
    }

    // last char
    *bytes_in_new_line = iter_bytes;

    // XXX special case for all block as a continuous string. Although it's not end yet, it forms a block.
    if(offset_line + *bytes_in_new_line == line_size) return S_OK;

    return S_ERR;
}

Err
pttdb_count_lines(char *content, int len, int *n_line)
{
    int tmp_n_line = 0;
    char *p_content = content;
    for(int i = 0; i < len - 1; i++, p_content++) {
        if(*p_content == '\n') {
            tmp_n_line++;
        }
    }
    *n_line = tmp_n_line;

    return S_OK;
}

Err
safe_free(void **a)
{
    if(!(*a)) return S_OK;
    
    free(*a);
    *a = NULL;
    return S_OK;
}

Err
form_rand_list(int n, int **p_rand_list) {
    *p_rand_list = malloc(sizeof(int) * n);
    if(!p_rand_list) return S_ERR;

    int *rand_list = *p_rand_list;

    int *available_pos = malloc(sizeof(int) * n);
    for(int i = 0; i < n; i++) {
        available_pos[i] = i;
    }

    int n_available_pos = n;
    int idx;
    int pos;
    int tmp;        
    for(int i = 0; i < n; i++, n_available_pos--) {
        idx = random() % n_available_pos;
        pos = available_pos[idx];
        rand_list[pos] = i;

        //fprintf(stderr, "pttdb_misc.form_rand_list: (%d/%d): idx: %d pos: %d n_available_pos: %d\n", i, n, idx, pos, n_available_pos);        

        // swap with last one
        tmp = pos;
        available_pos[idx] = available_pos[n_available_pos - 1];
        available_pos[n_available_pos - 1] = tmp;
    }

    //fprintf(stderr, "pttdb_misc.form_rand_list: to safe_free\n");

    safe_free((void **)&available_pos);

    return S_OK;
}