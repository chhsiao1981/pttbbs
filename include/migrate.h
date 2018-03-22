/* $Id$ */
#ifndef MIGRATE_H
#define MIGRATE_H

#include "ptterr.h"
#include "pttdb_util.h"
#include "pttdb_uuid.h"

#ifdef __cplusplus
extern "C" {
#endif

Err migrate_db_to_file(UUID main_id, const char *fpath, int saveheader);

#ifdef __cplusplus
}
#endif

#endif /* MIGRATE_H */
