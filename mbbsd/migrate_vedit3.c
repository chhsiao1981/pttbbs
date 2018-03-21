#include "migrate.h"
#include "migrate_internal.h"

int
migrate_vedit3(const char *fpath, int saveheader, char title[STRLEN], int flags)
{
    Err error_code = S_OK;

    // migrate file to db
    if(*fpath) {
       error_code = migrate_file_to_db(fpath, main_id);
    }

    // do quote
    if(!error_code && *quote_file) {
        error_code = migrate_do_quote(quote_file);
    }

    if(!error_code) {
        error_code = vedit3(main_id, title, current_text_line, flags, &money);
    }

    if(!error_code) {
        error_code = migrate_db_to_file(main_id, fpath, saveheader, title, flags);
    }

    if(error_code) {
        return EDIT_ABORTED;
    }

    return money;
}
