/* $Id$ */
#ifndef PROTO_EXTRA_H
#define PROTO_EXTRA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>

static int detect_attr(const char *ps, size_t len);

static void edit_outs_attr(const char *text, int attr);


#ifdef __cplusplus
}
#endif

#endif /* PROTO_EXTRA_H */
