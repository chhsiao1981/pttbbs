#include "cmpttui/vedit3.h"
#include "cmpttui/vedit3_private.h"

VEdit3EditorStatus VEDIT3_EDITOR_STATUS = {
    true,                // is-insert
    false,               // is-ansi
    false,               // is-indent
    false,               // is-phone
    false,               // is-raw
    0,                   // current-line
    0,                   // current-col
    0,                   // edit_margin
    0                    // last-margin 
};

VEdit3State VEDIT3_STATE = {};
VEdit3State VEDIT3_BUFFER_STATE = {};

FileInfo VEDIT3_FILE_INFO = {};

VEdit3BufferInfo VEDIT3_BUFFER_INFO = {};
VEdit3Buffer *VEDIT3_DISP_TOP_LINE_BUFFER = NULL;

char *_CONST_VEDIT3_LOADINGS[] = {
    ANSI_COLOR(0; 37)".",
    ANSI_COLOR(0; 37)".",
    ANSI_COLOR(0; 37)"."
};
char *_VEDIT3_LOADING_DOT0 = NULL;
char *_VEDIT3_LOADING_DOT1 = NULL;
char *_VEDIT3_LOADING_DOT2 = NULL;

char VEDIT3_EMPTY_LINE[] = "\n";
char VEDIT3_END_LINE[] = "~\n";

int
vedit3_wrapper(const char *fpath, int saveheader, char title[TTLEN + 1], int flags, fileheader_t *fhdr, boardheader_t *bp)
{
    int money = 0;

    char poster[IDLEN + 1] = {};
    char board[IDLEN + 1] = {};
    char origin[MAX_ORIGIN_LEN] = {};
    char web_link[MAX_WEB_LINK_LEN] = {};
    char title_d[TTLEN + 1] = {};
    aidu_t aid = 0;
    time64_t create_milli_timestamp = 0;

    UUID main_id = {};

    Err error_code = migrate_get_file_info(fhdr, bp, poster, board, title_d, origin, &aid, web_link, &create_milli_timestamp);
    if (error_code) return EDIT_ABORTED;

    fprintf(stderr, "vedit3.vedit3_wrapper: poster: %s board: %s title: %s title_d: %s origin: %s aid: %lu web_link: %s create_milli_timestamp: %lu\n", poster, board, title, title_d, origin, aid, web_link, create_milli_timestamp);

    error_code = migrate_file_to_pttdb(fpath, poster, board, title, origin, aid, web_link, create_milli_timestamp, main_id);

    error_code = vedit3(main_id, title, flags, &money);
    if (error_code) return EDIT_ABORTED;

    error_code = migrate_pttdb_to_file(main_id, fpath);
    if (error_code) return EDIT_ABORTED;

    return money;
}

/**
 * @brief
 * @details REPL 環境.
 *          1. user 的 input.
 *          2. 根據 user 的 input. 丟到 expected state 裡.
 *
 * @param main_id [description]
 * @param title [description]
 * @param edflags [description]
 * @param money [description]
 */
Err
vedit3(UUID main_id, char *title, int edflags, int *money)
{
    STATINC(STAT_VEDIT);

    Err error_code = _vedit3_init_user();
    if(error_code) return error_code;

    // init
    Err error_code = _vedit3_init_editor(main_id);
    fprintf(stderr, "vedit3.vedit3: after init editor: e: %d\n", error_code);

    // edit
    if(!error_code) {
        error_code = pttui_thread_set_expected_state(PTTUI_THREAD_STATE_EDIT);
    }

    if(!error_code) {
        error_code = _vedit3_disp_screen(0, b_lines - 1);
    }

    move(0, 0);

    if(!error_code) {
        error_code = _vedit3_repl(money);
    }

    // back to start
    Err error_code_set_expected_state = pttui_thread_set_expected_state(PTTUI_THREAD_STATE_START);
    if(error_code_set_expected_state) error_code = S_ERR_ABORT_BBS;

    Err error_code_sync = _vedit3_wait_buffer_thread_loop(PTTUI_THREAD_STATE_START);
    if (error_code_sync) error_code = S_ERR_ABORT_BBS;

    return error_code;
}


Err
_vedit3_init_editor(UUID main_id)
{
    Err error_code = _vedit3_edit_msg();
    if (error_code) return error_code;

    error_code = _vedit3_init_dots();
    if (error_code) return error_code;

    error_code = _vedit3_loading();
    if (error_code) return error_code;

    error_code = _vedit3_init_file_info(main_id); // XXX may result in race-condition
    fprintf(stderr, "vedit3._vedit3_init_editor: after init file_info: e: %d\n", error_code);
    if (error_code) return error_code;

    error_code = _vedit3_set_expected_state(main_id, PTTDB_CONTENT_TYPE_MAIN, VEDIT3_FILE_INFO.main_content_id, 0, 0, 0, b_lines);
    fprintf(stderr, "vedit3._vedit3_init_editor: after vedit3 set expected state: e: %d\n", error_code);
    if (error_code) return error_code;

    error_code = pttui_thread_set_expected_state(PTTUI_THREAD_STATE_INIT_EDIT);
    fprintf(stderr, "vedit3._vedit3_init_editor: after pttui set expected state: e: %d\n", error_code);
    if (error_code) return error_code;

    error_code = _vedit3_wait_buffer_init();

    return error_code;
}

Err
_vedit3_init_user()
{

    VEDIT3_EDITOR_STATUS.mode0 = currutmp->mode;
    VEDIT3_EDITOR_STATUS.destuid0 = currutmp->destuid;

    currutmp->mode = EDITING;
    currutmp->destuid = currstat;    
}

Err
_vedit3_init_file_info(UUID main_id)
{
    // XXX disp-buffer and disp-screen may need old file-info?
    Err error_code = destroy_file_info(&VEDIT3_FILE_INFO);
    fprintf(stderr, "_vedit3_init_file_info: after destroy_file_info: e: %d\n", error_code);
    if (error_code) return error_code;

    error_code = construct_file_info(main_id, &VEDIT3_FILE_INFO);
    fprintf(stderr, "_vedit3_init_file_info: after get_file_info: e: %d\n", error_code);
    fprintf(stderr, "vedit3._vedit3_init_file_info: n_main_line: %d n_main_block: %d n_comment: %d\n", VEDIT3_FILE_INFO.n_main_line, VEDIT3_FILE_INFO.n_main_block, VEDIT3_FILE_INFO.n_comment);

    return error_code;
}

Err
_vedit3_set_expected_state(UUID main_id, enum PttDBContentType top_line_content_type, UUID top_line_id, int top_line_block_offset, int top_line_line_offset, int top_line_comment_offset, int n_window_line)
{

    Err error_code = pttui_thread_lock_wrlock(LOCK_VEDIT3_EXPECTED_STATE);
    if (error_code) return error_code;

    memcpy(VEDIT3_STATE.main_id, main_id, UUIDLEN);
    VEDIT3_STATE.top_line_content_type = top_line_content_type;
    memcpy(VEDIT3_STATE.top_line_id, top_line_id, UUIDLEN);
    VEDIT3_STATE.top_line_block_offset = top_line_block_offset;
    VEDIT3_STATE.top_line_line_offset = top_line_line_offset;
    VEDIT3_STATE.top_line_comment_offset = top_line_comment_offset;
    VEDIT3_STATE.n_window_line = n_window_line;

    pttui_thread_lock_unlock(LOCK_VEDIT3_EXPECTED_STATE);

    return S_OK;
}

Err
_vedit3_get_expected_state(VEdit3State *expected_state)
{
    Err error_code = pttui_thread_lock_rdlock(LOCK_VEDIT3_EXPECTED_STATE);
    if (error_code) return error_code;

    memcpy(expected_state, &VEDIT3_STATE, sizeof(VEdit3State));

    pttui_thread_lock_unlock(LOCK_VEDIT3_EXPECTED_STATE);

    return error_code;
}

Err
_vedit3_wait_buffer_init()
{
    Err error_code = S_OK;
    int ret_sleep = 0;
    VEdit3State current_state = {};

    struct timespec req = {0, NS_DEFAULT_SLEEP_VEDIT3_WAIT_BUFFER_INIT};
    struct timespec rem = {};

    int i = 0;
    for (i = 0; i < N_ITER_VEDIT3_WAIT_BUFFER_INIT; i++) {
        error_code = _vedit3_get_buffer_current_state(&current_state);
        if (error_code) break;

        fprintf(stderr, "vedit3._vedit3_wait_buffer_init: VEDIT3_STATE: (%d, %d, %d, %d) current_state: (%d, %d, %d, %d)\n", VEDIT3_STATE.top_line_content_type, VEDIT3_STATE.top_line_block_offset, VEDIT3_STATE.top_line_line_offset, VEDIT3_STATE.n_window_line, current_state.top_line_content_type, current_state.top_line_block_offset, current_state.top_line_line_offset, current_state.n_window_line);

        if (!memcmp(&VEDIT3_STATE, &current_state, sizeof(VEdit3State))) break; // XXX maybe re-edit too quickly

        ret_sleep = nanosleep(&req, &rem);
        if (ret_sleep) {
            error_code = S_ERR;
            break;
        }
    }
    if (i == N_ITER_VEDIT3_WAIT_BUFFER_INIT) error_code = S_ERR_BUSY;

    return error_code;
}


Err
_vedit3_wait_buffer_thread_loop(enum PttUIThreadState expected_state)
{
    Err error_code = S_OK;
    int ret_sleep = 0;
    enum PttUIThreadState current_state = PTTUI_THREAD_STATE_START;

    struct timespec req = {0, NS_DEFAULT_SLEEP_VEDIT3_WAIT_BUFFER_THREAD_LOOP};
    struct timespec rem = {};

    int i = 0;
    for (i = 0; i < N_ITER_VEDIT3_WAIT_BUFFER_THREAD_LOOP; i++) {
        error_code = pttui_thread_get_current_state(PTTUI_THREAD_DISP_BUFFER, &current_state);
        if (error_code) break;

        if (expected_state == current_state) break; // XXX maybe re-edit too quickly

        ret_sleep = nanosleep(&req, &rem);
        if (ret_sleep) {
            error_code = S_ERR;
            break;
        }
    }
    if (i == N_ITER_VEDIT3_WAIT_BUFFER_THREAD_LOOP) error_code = S_ERR_BUSY;

    return error_code;
}

/*********
 * VEdit3 repl
 *********/
Err
_vedit3_repl(int *money)
{
    *money = 0;

    fprintf(stderr, "vedit3._vedit3_repl: start\n");

    /*
    Err error_code = S_OK;
    bool is_redraw_everything = false;    
    bool is_end = false;

    struct timespec req = {0, NS_DEFAULT_SLEEP_VEDIT3_REPL};
    struct timespec rem = {};

    int ret_sleep = 0;
    while(true) {
        // action
        // check healthy
        error_code = _vedit3_check_healthy(&is_redraw_everything);
        if(error_code) break;

        error_code = _vedit3_action_to_store(&is_end);
        if(error_code) break;

        error_code = _vedit3_store_to_render();
        if(error_code) break;

        if(is_end) break;


        ret_sleep = nanosleep(&req, &rem);
        if(ret_sleep) {
            error_code = S_ERR_SLEEP;
            break;
        }
    }
    */

    struct timespec req = {10, 0};
    struct timespec rem = {};

    ret_sleep = nanosleep(&req, &rem);

    return error_code;
}

Err
_vedit3_store_to_render()
{
    // re-render based on line for now
    VEdit3Buffer *p_buffer = VEDIT3_DISP_TOP_LINE_BUFFER;
    int current_line = 0;
    int *p_modified_line = VEDIT3_EDITOR_STATUS.modified_line;


    for(current_line = 0; current_line < VEDIT3_EDITOR_STATUS.n_modified_line && p_buffer; current_line++, p_modified_line++, p_buffer = p_buffer->next) {
        if(!(*p_modified_line)) continue;

        mvouts(current_line, 0, p_buffer->buf);
    }

    refresh();
}




/*********
 * VEdit3 disp screen
 *********/

Err
_vedit3_disp_screen(int start_line, int end_line)
{
    Err error_code = pttui_thread_lock_rdlock(LOCK_VEDIT3_BUFFER_INFO);
    if(error_code) return error_code;

    VEdit3Buffer *p_buffer = VEDIT3_DISP_TOP_LINE_BUFFER;
    fprintf(stderr, "vedit3._vedit3_disp_screen: VEDIT3_DISP_TOP_LINE_BUFFER: %lu\n", VEDIT3_DISP_TOP_LINE_BUFFER);
    int i = 0;
    for(i = 0; i < start_line && p_buffer; i++, p_buffer = p_buffer->next);
    if(i != start_line) error_code = S_ERR;

    fprintf(stderr, "vedit3._vedit3_disp_screen: to for-loop: i: %d start_line: %d end_line: %d e: %d\n", i, start_line, end_line, error_code);

    if(!error_code) {
        for(; i <= end_line && p_buffer; i++, p_buffer = p_buffer->next) {
            if(!p_buffer->buf) {
                fprintf(stderr, "vedit3._vedit3_disp_screen: (%d/%d): unable to get buf\n", i, end_line);
                mvouts(i, 0, VEDIT3_EMPTY_LINE);
                continue;
            }
            fprintf(stderr, "vedit3._vedit3_disp_screen: (%d/%d): buf: %s\n", i, end_line, p_buffer->buf);
            mvouts(i, 0, p_buffer->buf);
        }
    }

    fprintf(stderr, "vedit3._vedit3_disp_screen: after for-loop: i: %d start_line: %d end_line: %d e: %d\n", i, start_line, end_line, error_code);

    if(!error_code) {
        for(; i <= end_line; i++) {
            fprintf(stderr, "vedit3._vedit3_disp_screen: (%d/%d): end\n", i, end_line);
            mvouts(i, 0, VEDIT3_END_LINE);
        }
    }

    refresh();

    // free
    Err error_code_lock = pttui_thread_lock_unlock(LOCK_VEDIT3_BUFFER_INFO);
    if(error_code_lock) error_code = S_ERR_ABORT_BBS;

    return error_code;
}


/*********
 * VEdit3 Buffer
 *********/

Err
_vedit3_get_buffer_current_state(VEdit3State *state)
{
    Err error_code = pttui_thread_lock_rdlock(LOCK_VEDIT3_BUFFER_STATE);
    if (error_code) return error_code;

    memcpy(state, &VEDIT3_BUFFER_STATE, sizeof(VEdit3State));

    // free
    error_code = pttui_thread_lock_unlock(LOCK_VEDIT3_BUFFER_STATE);
    if (error_code) return error_code;

    return S_OK;
}

Err
_vedit3_set_buffer_current_state(VEdit3State *state)
{
    Err error_code = pttui_thread_lock_wrlock(LOCK_VEDIT3_BUFFER_STATE);
    if (error_code) return error_code;

    memcpy(&VEDIT3_BUFFER_STATE, state, sizeof(VEdit3State));

    // free
    error_code = pttui_thread_lock_unlock(LOCK_VEDIT3_BUFFER_STATE);
    if (error_code) return error_code;

    return S_OK;
}

Err
vedit3_init_disp_buffer()
{
    Err error_code = S_OK;

    VEdit3State expected_state = {};
    error_code = _vedit3_get_expected_state(&expected_state);
    if (error_code) return error_code;

    if (!memcmp(expected_state.main_id, VEDIT3_FILE_INFO.main_id, UUIDLEN) &&
        !memcmp(expected_state.main_id, VEDIT3_BUFFER_STATE.main_id, UUIDLEN)) return S_OK;

    char *_uuid = display_uuid(expected_state.top_line_id);    
    fprintf(stderr, "vedit3.vedit3_init_disp_buffer: to resync all vedit3 buffer info: e: %d state: (content_type: %d, block_offset: %d, line_offset: %d, comment_offset: %d, n_window_line: %d top_line_id: %s)\n", error_code, expected_state.top_line_content_type, expected_state.top_line_block_offset, expected_state.top_line_line_offset, expected_state.top_line_comment_offset, expected_state.n_window_line, _uuid);
    safe_free(&_uuid);

    error_code = resync_all_vedit3_buffer_info(&VEDIT3_BUFFER_INFO, &expected_state, &VEDIT3_FILE_INFO, &VEDIT3_DISP_TOP_LINE_BUFFER);
    fprintf(stderr, "vedit3.vedit3_init_disp_buffer: after resync all vedit3_buffer_info: e: %d\n", error_code);
    if (error_code) return error_code;

    error_code = _vedit3_set_buffer_current_state(&expected_state);
    fprintf(stderr, "vedit3.vedit3_init_disp_buffer: after vedit3_set_buffer_current_state: e: %d\n", error_code);
    if (error_code) return error_code;

    return S_OK;
}

Err
vedit3_disp_buffer()
{
    Err error_code = S_OK;

    VEdit3State expected_state = {};

    //fprintf(stderr, "vedit3_disp_buffer: to get expected state\n");

    error_code = _vedit3_get_expected_state(&expected_state);

    // sync disp buffer to tmp_disp_buffer while checking the alignment of orig_expected_state and new_expected_state
    /*
    if (!error_code) {
        error_code = _vedit3_sync_disp_buffer(&expected_state);
    }
    */


    //fprintf(stderr, "vedit3_disp_buffer: end: e: %d\n", error_code);

    return error_code;
}

Err
_vedit3_sync_disp_buffer(VEdit3State *expected_state)
{
    Err error_code = S_OK;
    bool is_need_sync = false;

    /*
    error_code = _vedit3_sync_disp_buffer_is_need_sync_primitive(expected_state, &is_need_sync);
    if (error_code) return error_code;
    if (!is_need_sync) return S_OK;

    // main-id diff: resync all
    if(memcmp(expected_state->main_id, _VEDIT3_CURRENT_STATE.main_id, UUIDLEN)) {
        error_code = _vedit3_sync_disp_buffer_resync_all(expected_state);
        return error_code;
    }
    */

    // get buffer line
    /*
    error_code = _vedit3_get_buffer_line(expected_state->top_line_content_type, expected_state->top_line_block_offset, expected_state->top_line_line_offset, &p_top_buffer_line, &offset_buffer_line);
    if(error_code) return error_code;

    // if top_buffer_line exists in current buffer and top-buffer-line is in safe range
    //     => we just need to set top-line
    if(p_top_buffer_line && offset_buffer_line >= DEFAULT_VEDIT3_MIN_OFFSET_BUFFER_LINE && offset_buffer_line < _VEDIT3_CURRENT_BUFFER.n_buffer_line - DEFAULT_VEDIT3_MIN_OFFSET_BUFFER_LINE) {
        error_code = _vedit3_sync_disp_buffer_set_expected_top_line(p_top_buffer_line);
        return error_code;
    }

    error_code = _vedit3_sync_disp_buffer_sync(expected_state);
    */
    return error_code;
}

/*********
 * VEdit3 Misc
 *********/
/**
 * @brief 
 * @details ref: edit_msg in edit.c
 */
Err
_vedit3_edit_msg()
{    
    char buf[MAX_TEXTLINE_SIZE] = {};
    int n = VEDIT3_EDITOR_STATUS.current_col;

    /*
    if (VEDIT3_EDITOR_STATUS.is_ansi)    
    //n = n2ansi(n, curr_buf->currline);

    if (VEDIT3_EDITOR_STATUS.is_phone) show_phone_mode_panel();
    */

    snprintf(buf, sizeof(buf),
        FOOTER_VEDIT3_INFIX
        FOOTER_VEDIT3_POSTFIX,
        VEDIT3_EDITOR_STATUS.is_insert ? VEDIT3_PROMPT_INSERT : VEDIT3_PROMPT_REPLACE,
        VEDIT3_EDITOR_STATUS.is_ansi ? 'A' : 'a',
        VEDIT3_EDITOR_STATUS.is_indent ? 'I' : 'i',
        VEDIT3_EDITOR_STATUS.is_phone ? 'P' : 'p',
        VEDIT3_EDITOR_STATUS.is_raw ? 'R' : 'r',
        VEDIT3_EDITOR_STATUS.current_line + 1,
        n + 1);

    vs_footer(FOOTER_VEDIT3_PREFIX, buf);
}

Err
_vedit3_init_dots()
{
    _VEDIT3_LOADING_DOT0 = _CONST_VEDIT3_LOADINGS[0];
    _VEDIT3_LOADING_DOT1 = _CONST_VEDIT3_LOADINGS[1];
    _VEDIT3_LOADING_DOT2 = _CONST_VEDIT3_LOADINGS[2];

    return S_OK;
}

Err
_vedit3_loading()
{
    char buf[MAX_TEXTLINE_SIZE] = {};
    sprintf(buf, LOADING "%s%s%s%s\n", _VEDIT3_LOADING_DOT0, _VEDIT3_LOADING_DOT1, _VEDIT3_LOADING_DOT2, ANSI_RESET);
    mvouts(b_lines - 1, 0, buf);
    refresh();
    _vedit3_loading_rotate_dots();

    fprintf(stderr, "vedit3._vedit3_loading: buf: %s b_lines: %d t_lines: %d\n", buf, b_lines, t_lines);

    return S_OK;
}

Err
_vedit3_loading_rotate_dots()
{
    char *tmp = _VEDIT3_LOADING_DOT0;
    _VEDIT3_LOADING_DOT0 = _VEDIT3_LOADING_DOT2;
    _VEDIT3_LOADING_DOT2 = _VEDIT3_LOADING_DOT1;
    _VEDIT3_LOADING_DOT1 = tmp;

    return S_OK;
}
