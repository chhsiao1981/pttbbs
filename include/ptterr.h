/* $Id$ */
#ifndef PTTERR_H
#define PTTERR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef char Err;

#define S_OK 0
#define S_ERR -1                   // 255
#define S_ERR_MALLOC -2            // 254
#define S_ERR_ALREADY_EXISTS -3    // 253
#define S_ERR_NOT_EXISTS -4        // 252
#define S_ERR_FOUND_MULTI -5       // 251
#define S_ERR_BUFFER_LEN -6        // 250

#ifdef __cplusplus
}
#endif

#endif /* PTTERR_H */