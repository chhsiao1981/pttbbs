/* $Id$ */
#ifndef PTTDB_FILE_H
#define PTTDB_FILE_H

#include "ptterr.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

#ifndef PTTDB_FILE_PREFIX_DIR
#define PTTDB_FILE_PREFIX_DIR "storage"
#endif

Err init_pttdb_file();
Err pttdb_file_get_main_dir_prefix_name(UUID main_id, char *dirname);
Err pttdb_file_attach_main_dir(UUID main_id, char *dirname);
Err pttdb_file_get_main_dir_name(UUID main_id, char *dirname);
Err pttdb_file_get_data(UUID main_id, PttDBContentType content_type, UUID content_id, int block_id, int file_id, char **buf, int *len);
Err pttdb_file_save_data(UUID main_id, PttDBContentType content_type, UUID content_id, int block_id, int file_id, char *buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* PTTDB_FILE_H */

