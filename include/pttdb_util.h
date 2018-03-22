/* $Id$ */
#ifndef PTTDB_UTIL_H
#define PTTDB_UTIL_H

#include "ptterr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_BUF_SIZE 8192

Err get_line_from_buf(char *p_buf, int offset_buf, int bytes, char *p_line, int offset_line, int *bytes_in_new_line);
Err pttdb_count_lines(char *content, int len, int *n_line);

Err safe_free(void **a);
Err safe_free_b_list(bson_t ***b, int n);

Err form_rand_list(int n, int **rand_list);

#ifdef __cplusplus
}
#endif

#endif /* PTTDB_H */