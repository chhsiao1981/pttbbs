/* $Id$ */
#ifndef PTTDB_UUID_H
#define PTTDB_UUID_H

#include "ptterr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UUIDLEN 16
#define _UUIDLEN 16
#define DISPLAY_UUIDLEN 24

#define N_GEN_UUID_WITH_DB 10


// XXX hack for time64_t and UUID
typedef long int time64_t;

typedef unsigned char UUID[UUIDLEN];
typedef unsigned char _UUID[_UUIDLEN];

extern const UUID EMPTY_ID;

/**********
 * Milli-timestamp
 **********/
Err get_milli_timestamp(time64_t *milli_timestamp);

/**********
 * UUID
 **********/
Err gen_uuid(UUID uuid);
Err gen_uuid_with_db(int collection, UUID uuid);
Err gen_content_uuid_with_db(int collection, UUID uuid);

Err uuid_to_milli_timestamp(UUID uuid, time64_t *milli_timestamp);


#ifdef __cplusplus
}
#endif

#endif /* PTTDB_UUID_H */