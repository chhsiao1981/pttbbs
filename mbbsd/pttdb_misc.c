#include "pttdb.h"
#include "pttdb_internal.h"

/**********
 * Get line from buf
 **********/

/**
 * @brief Try to get a line (ending with \r\n) from buffer.
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
get_line_from_buf(char *p_buf, int offset_buf, int bytes, char *p_line, int offset_line, int *bytes_in_new_line)
{

    // check the end of buf
    if (offset_buf >= bytes) {
        *bytes_in_new_line = 0;

        return S_ERR;
    }

    // init p_buf offset
    p_buf += offset_buf;
    p_line += offset_line;

    // check bytes in line and in buf.
    if (offset_line && p_line[-1] == '\r' && p_buf[0] == '\n') {
        *p_line = '\n';
        *bytes_in_new_line = 1;

        return S_OK;
    }

    // check \r\n in buf.
    int max_new_lines = MAX_BUF_SIZE - offset_line;
    int iter_bytes = (bytes - offset_buf <= max_new_lines) ? (bytes - offset_buf) : max_new_lines;
    int end_bytes = iter_bytes + offset_buf;
    for (int i = offset_buf; i < end_bytes - 1; i++) {
        if (*p_buf == '\r' && *(p_buf + 1) == '\n') {
            *p_line = '\r';
            *(p_line + 1) = '\n';
            *bytes_in_new_line = i - offset_buf + 1 + 1;

            return S_OK;
        }

        *p_line++ = *p_buf++;
    }

    // last char
    *p_line++ = *p_buf++;
    *bytes_in_new_line = end_bytes - offset_buf;

    // XXX special case for all block as a continuous string. Although it's not end yet, it forms a block.
    if(*bytes_in_new_line == max_new_lines) return S_OK;

    return S_ERR;
}

Err
pttdb_count_lines(char *content, int len, int *n_line)
{
    int tmp_n_line = 0;
    char *p_content = content;
    for(int i = 0; i < len - 1; i++, p_content++) {
        if(*p_content == '\r' && *(p_content + 1) == '\n') {
            tmp_n_line++;
        }
    }
    *n_line = tmp_n_line;

    return S_OK;
}

Err
safe_free_b_list(bson_t ***b, int n)
{
    bson_t **p_b = *b;
    for(int i = 0; i < n; i++, p_b++) {
        bson_safe_destroy(p_b);
    }
    safe_free((void **)b);

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

char *
_display_uuid(UUID uuid)
{
    char *result = malloc(DISPLAY_UUIDLEN + 1);
    result[DISPLAY_UUIDLEN] = 0;

    b64_ntop(uuid, UUIDLEN, (char *)result, DISPLAY_UUIDLEN);

    return result;
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

        fprintf(stderr, "pttdb_misc.form_rand_list: (%d/%d): idx: %d pos: %d n_available_pos: %d\n", i, n, idx, pos, n_available_pos);        

        // swap with last one
        tmp = pos;
        available_pos[idx] = available_pos[n_available_pos - 1];
        available_pos[n_available_pos - 1] = tmp;
    }

    fprintf(stderr, "pttdb_misc.form_rand_list: to safe_free\n");

    safe_free((void **)&available_pos);

    return S_OK;
}