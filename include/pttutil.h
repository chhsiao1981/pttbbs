/* $Id$ */

#ifndef PTTUTIL_H
#define PTTUTIL_H

#include "ptterr.h"

Err get_line_from_buf(char *p_buf, int offset_buf, int bytes, char *p_line, int offset_line, int *bytes_in_new_line);


#endif