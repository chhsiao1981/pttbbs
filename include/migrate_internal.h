/* $Id$ */
#ifndef MIGRATE_INTERNAL_H
#define MIGRATE_INTERNAL_H

#include "pttdb.h"

#ifdef __cplusplus
extern "C" {
#endif

Err _migrate_main_content_to_file(MainHeader main_header, FILE *fp);
Err _migrate_main_content_to_file_core(UUID content_id, FILE *fp, int start_block_id, int next_block_id);
Err _migrate_comment_comment_reply_by_main_to_file(UUID main_header, FILE *fp);
Err _migrate_comment_comment_reply_by_main_to_file_core(bsont_t **b_comments, int n_comment, FILE *fp);


#ifdef __cplusplus
}
#endif

#endif /* MIGRATE_INTERNAL_H */
