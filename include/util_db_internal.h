/* $Id$ */
#ifndef UTIL_DB_INTERNAL_H
#define UTIL_DB_INTERNAL_H

#include <mongoc.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "ptterr.h"

/**********
 * Mongo
 **********/

// XXX NEVER USE UNLESS IN TEST
Err _DB_FORCE_DROP_COLLECTION(int collection);

#endif /* UTIL_DB_INTERNAL_H */