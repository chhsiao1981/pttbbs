/* $Id$ */
#ifndef PTTDB_UTIL_H
#define PTTDB_UTIL_H

#include "ptterr.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "osdep.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "config.h"

#ifndef INCLUDE_VAR_H
    #include "var.h"
#endif

#define MAX_BUF_SIZE 8192

Err get_line_from_buf(char *p_buf, int offset_buf, int buf_size, char *p_line, int offset_line, int line_size, int *bytes_in_new_line);
Err pttdb_count_lines(char *content, int len, int *n_line);

Err safe_free(void **a);

Err form_rand_list(int n, int **rand_list);

#ifdef __cplusplus
}
#endif

#endif /* PTTDB_UTIL_H */