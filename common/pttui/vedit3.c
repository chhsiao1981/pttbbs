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
    0,                   // last-margin

    false,               // is own lock buffer info
    false,               // is redraw everything

    NULL,                // current-buffer

    0,                   // mode0
    0                    // destuid0
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
int LEN_VEDIT3_EMPTY_LINE = 2;
char VEDIT3_END_LINE[] = "~\n";
int LEN_VEDIT3_END_LINE = 3;

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
    if (error_code) return error_code;

    // init
    error_code = _vedit3_init_editor(main_id);
    fprintf(stderr, "vedit3.vedit3: after init editor: e: %d\n", error_code);

    // edit
    if (!error_code) {
        error_code = pttui_thread_set_expected_state(PTTUI_THREAD_STATE_EDIT);
    }

    if(!error_code) {
        error_code = _vedit3_repl_init();
    }

    // repl
    if (!error_code) {
        error_code = _vedit3_repl(money);
    }

    // back to start
    Err error_code_set_expected_state = pttui_thread_set_expected_state(PTTUI_THREAD_STATE_START);
    if (error_code_set_expected_state) error_code = S_ERR_ABORT_BBS;

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

    return S_OK;
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
    Err error_code = S_OK;
    *money = 0;

    fprintf(stderr, "vedit3._vedit3_repl: start\n");

    // repl init

    bool is_redraw_everything = false;
    bool is_end = false;

    struct timespec req = {0, NS_DEFAULT_SLEEP_VEDIT3_REPL};
    struct timespec rem = {};

    int ret_sleep = 0;
    while(true) {
        // action
        // check healthy
        error_code = _vedit3_check_healthy();
        if(error_code) break;

        error_code = vedit3_action_to_store(&is_end);
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

    /*
    struct timespec req = {10, 0};
    struct timespec rem = {};

    int ret_sleep = nanosleep(&req, &rem);
    */

    return error_code;
}

Err
_vedit3_repl_init() {
    Err error_code = S_OK;

    // init editor-status
    bzero(&VEDIT3_EDITOR_STATUS, sizeof(VEDIT3_EDITOR_STATUS));

    error_code = vedit3_lock_buffer_info();
    VEDIT3_EDITOR_STATUS.current_buffer = VEDIT3_DISP_TOP_LINE_BUFFER;

    // disp-screen
    error_code = _vedit3_disp_screen(0, b_lines - 1);

    if (!error_code) {
        move(VEDIT3_EDITOR_STATUS.current_line, VEDIT3_EDITOR_STATUS.current_col);
        refresh();
    }

    // free
    Err error_code_lock = vedit3_unlock_buffer_info();
    if (error_code_lock) error_code = S_ERR_ABORT_BBS;

    return S_OK;
}

Err
_vedit3_check_healthy()
{
    return S_OK;
}

Err
_vedit3_store_to_render()
{
    Err error_code = S_OK;
    if(VEDIT3_EDITOR_STATUS.is_redraw_everything) {
        VEDIT3_EDITOR_STATUS.is_redraw_everything = false;
        error_code = _vedit3_disp_screen(0, b_lines - 1);
    }

    move(VEDIT3_EDITOR_STATUS.current_line, VEDIT3_EDITOR_STATUS.current_col);
    refresh();
    
    return error_code;
}

/*********
 * VEdit3 disp screen
 *********/

/**
 * @brief refresh screen from start-line, add VEDIT3_END_LINE if end_line is not the end of the screen.
 * @details refresh screen from start-tline.
 *          add VEDIT3_END_LINE if end_line is not the end of the screen
 * 
 * @param start_line [description]
 * @param end_line the end-of-line to refresh(included)
 */
Err
_vedit3_disp_screen(int start_line, int end_line)
{
    Err error_code = vedit3_lock_buffer_info();
    if (error_code) return error_code;

    // assuming VEDIT3_DISP_TOP_LINE_BUFFER is within expectation, with lock
    VEdit3Buffer *p_buffer = VEDIT3_DISP_TOP_LINE_BUFFER;

    // iterate to the start line    
    int i = 0;
    for (i = 0; i < start_line && p_buffer; i++, p_buffer = p_buffer->next);
    if (i != start_line) error_code = S_ERR;

    // disp between start_line and end_line (end-line included)
    if (!error_code) {
        for (; i <= end_line && p_buffer; i++, p_buffer = p_buffer->next) {
            if (!p_buffer->buf) {
                error_code = _vedit3_disp_line(i, VEDIT3_EMPTY_LINE, LEN_VEDIT3_EMPTY_LINE);
                if (error_code) break;

                continue;
            }
            error_code = _vedit3_disp_line(i, p_buffer->buf, p_buffer->len);
            if (error_code) break;
        }
    }

    fprintf(stderr, "vedit3._vedit3_disp_screen: after for-loop: i: %d start_line: %d end_line: %d e: %d\n", i, start_line, end_line, error_code);

    // disp 
    if (!error_code) {
        for (; i <= end_line; i++) {
            error_code = _vedit3_disp_line(i, VEDIT3_END_LINE, LEN_VEDIT3_END_LINE);
            if (error_code) break;
        }
    }

    if (!error_code) {
        refresh();
    }

    // free    
    Err error_code_lock = vedit3_unlock_buffer_info();
    if (error_code_lock) error_code = S_ERR_ABORT_BBS;

    return error_code;
}

/**
 * @brief
 * @details ref: the end of vedit2 in edit.c
 * 
 * @param line [description]
 * @param buf [description]
 * @param len [description]
 */
Err
_vedit3_disp_line(int line, char *buf, int len)
{
    Err error_code = S_OK;

    move(line, 0);
    clrtoeol();
    int attr = (int)VEDIT3_ATTR_NORMAL;
    int detected_attr = 0;

    if (VEDIT3_EDITOR_STATUS.is_ansi) {
        outs(buf);
    }
    else {
        error_code = _vedit3_detect_attr(buf, len, &detected_attr);
        attr |= detected_attr;
        _vedit3_edit_outs_attr_n(buf + VEDIT3_EDITOR_STATUS.edit_margin, strlen(buf + VEDIT3_EDITOR_STATUS.edit_margin), attr);
    }

    return S_OK;
}


/**
 * @brief [brief description]
 * @details ref: edit_outs_attr_n in mbbsd/edit.c
 *
 * @param text [description]
 * @param n [description]
 * @param attr [description]
 */
Err
_vedit3_edit_outs_attr_n(const char *text, int n, int attr)
{
    int    column = 0;
    unsigned char inAnsi = 0;
    unsigned char ch;
    int doReset = 0;
    const char *reset = ANSI_RESET;

    // syntax attributes
    char fComment = 0,
         fSingleQuote = 0,
         fDoubleQuote = 0,
         fSquareQuote = 0,
         fWord = 0;

    // movie syntax rendering
#ifdef ENABLE_PMORE_ASCII_MOVIE_SYNTAX
    char movie_attrs[WRAPMARGIN + 10] = {0};
    char *pmattr = movie_attrs, mattr = 0;
#endif

#ifdef COLORED_SELECTION
    if ((attr & VEDIT3_ATTR_SELECTED) &&
            (attr & ~VEDIT3_ATTR_SELECTED))
    {
        reset = ANSI_COLOR(0; 7; 36);
        doReset = 1;
        outs(reset);
    }
    else
#endif // if not defined, color by  priority - selection first
        if (attr & VEDIT3_ATTR_SELECTED)
        {
            reset = ANSI_COLOR(0; 7);
            doReset = 1;
            outs(reset);
        }
        else if (attr & VEDIT3_ATTR_MOVIECODE)
        {
            reset = ANSI_COLOR(0; 36);
            doReset = 1;
            outs(reset);
#ifdef ENABLE_PMORE_ASCII_MOVIE_SYNTAX
            syn_pmore_render((char*)text, n, movie_attrs);
#endif
        }
        else if (attr & VEDIT3_ATTR_BBSLUA)
        {
            reset = ANSI_COLOR(0; 1; 31);
            doReset = 1;
            outs(reset);
        }
        else if (attr & VEDIT3_ATTR_COMMENT)
        {
            reset = ANSI_COLOR(0; 1; 34);
            doReset = 1;
            outs(reset);
        }

#ifdef DBCSAWARE
    /* 0 = N/A, 1 = leading byte printed, 2 = ansi in middle */
    unsigned char isDBCS = 0;
#endif

    while ((ch = *text++) && (++column < t_columns) && n-- > 0)
    {
#ifdef ENABLE_PMORE_ASCII_MOVIE_SYNTAX
        mattr = *pmattr++;
#endif

        if (inAnsi == 1)
        {
            if (ch == ESC_CHR)
                outc('*');
            else
            {
                outc(ch);

                if (!ANSI_IN_ESCAPE(ch))
                {
                    inAnsi = 0;
                    outs(reset);
                }
            }

        }
        else if (ch == ESC_CHR)
        {
            inAnsi = 1;
#ifdef DBCSAWARE
            if (isDBCS == 1)
            {
                isDBCS = 2;
                outs(ANSI_COLOR(1; 33) "?");
                outs(reset);
            }
#endif
            outs(ANSI_COLOR(1) "*");
        }
        else
        {
#ifdef DBCSAWARE
            if (isDBCS == 1)
                isDBCS = 0;
            else if (isDBCS == 2)
            {
                /* ansi in middle. */
                outs(ANSI_COLOR(0; 33) "?");
                outs(reset);
                isDBCS = 0;
                continue;
            }
            else if (IS_BIG5_HI(ch))
            {
                isDBCS = 1;
                // peak next char
                if (n > 0 && *text == ESC_CHR)
                    continue;
            }
#endif

            // XXX Lua Parser!
            //if (!attr && curr_buf->synparser && !fComment)
            if (!attr && false && !fComment)
            {
                // syntax highlight!
                if (fSquareQuote) {
                    if (ch == ']' && n > 0 && *(text) == ']')
                    {
                        fSquareQuote = 0;
                        doReset = 0;
                        // directly print quotes
                        outc(ch); outc(ch);
                        text++, n--;
                        outs(ANSI_RESET);
                        continue;
                    }
                } else if (fSingleQuote) {
                    if (ch == '\'')
                    {
                        fSingleQuote = 0;
                        doReset = 0;
                        // directly print quotes
                        outc(ch);
                        outs(ANSI_RESET);
                        continue;
                    }
                } else if (fDoubleQuote) {
                    if (ch == '"')
                    {
                        fDoubleQuote = 0;
                        doReset = 0;
                        // directly print quotes
                        outc(ch);
                        outs(ANSI_RESET);
                        continue;
                    }
                } else if (ch == '-' && n > 0 && *(text) == '-') {
                    fComment = 1;
                    doReset = 1;
                    outs(ANSI_COLOR(0; 1; 34));
                } else if (ch == '[' && n > 0 && *(text) == '[') {
                    fSquareQuote = 1;
                    doReset = 1;
                    fWord = 0;
                    outs(ANSI_COLOR(1; 35));
                } else if (ch == '\'' || ch == '"') {
                    if (ch == '"')
                        fDoubleQuote = 1;
                    else
                        fSingleQuote = 1;
                    doReset = 1;
                    fWord = 0;
                    outs(ANSI_COLOR(1; 35));
                } else {
                    // normal words
                    if (fWord)
                    {
                        // inside a word.
                        if (--fWord <= 0) {
                            fWord = 0;
                            doReset = 0;
                            outc(ch);
                            outs(ANSI_RESET);
                            continue;
                        }
                    } else if (isalnum(tolower(ch)) || ch == '#') {
                        char attr[] = ANSI_COLOR(0; 1; 37);
                        int x = _vedit3_syn_lua_keyword(text - 1, n + 1, &fWord);
                        if (fWord > 0)
                            fWord --;
                        if (x != 0)
                        {
                            // sorry, fixed string here.
                            // 7 = *[0;1;3?
                            if (x < 0) {  attr[4] = '0'; x = -x; }
                            attr[7] = '0' + x;
                            outs(attr);
                            doReset = 1;
                        }
                        if (!fWord)
                        {
                            outc(ch);
                            outs(ANSI_RESET);
                            doReset = 0;
                            continue;
                        }
                    }
                }
            }
            outc(ch);

#ifdef ENABLE_PMORE_ASCII_MOVIE_SYNTAX
            // pmore Movie Parser!
            if (attr & VEDIT3_ATTR_MOVIECODE)
            {
                // only render when attribute was changed.
                if (mattr != *pmattr)
                {
                    if (*pmattr)
                    {
                        prints(ANSI_COLOR(1; 3 % d),
                               8 - ((mattr - 1) % 7 + 1) );
                    } else {
                        outs(ANSI_RESET);
                    }
                }
            }
#endif // ENABLE_PMORE_ASCII_MOVIE_SYNTAX
        }
    }

    // this must be ANSI_RESET, not "reset".
    if (inAnsi || doReset)
        outs(ANSI_RESET);

    return S_OK;
}

/**
 * @brief
 * @details ref: edit_ansi_outs_n in edit.c
 * 
 * @param str [description]
 * @param n [description]
 * @param attr [description]
 */
Err
_vedit3_edit_ansi_outs_n(const char *str, int n, int attr GCC_UNUSED)
{
    char c;
    while (n-- > 0 && (c = *str++)) {
        if (c == ESC_CHR && *str == '*')
        {
            // ptt prints
            /* Because moving within ptt_prints is too hard
             * let's just display it as-is.
             */
            outc('*');
        } else {
            outc(c);
        }
    }
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
 * @brief [brief description]
 * @details no need to worry much about repeated lock because:
 *          1. it's read-lock, can be locked by vedit3-main multiple times
 *          2. is-own-lock-buffer-info is just for the flag to ensure that vedit3-main owns the lock.
 * 
 * @param e [description]
 */
Err
vedit3_lock_buffer_info()
{
    Err error_code = pttui_thread_lock_rdlock(LOCK_VEDIT3_BUFFER_INFO);
    if(error_code) return error_code;
        
    VEDIT3_EDITOR_STATUS.is_own_lock_buffer_info = true;    

    return S_OK;
}

Err
vedit3_unlock_buffer_info()
{
    pthread_rwlock_t *p_lock = NULL;

    Err error_code = pttui_thread_lock_get_lock(LOCK_VEDIT3_BUFFER_INFO, &p_lock);

    if(!error_code && p_lock->__data.__nr_readers == 1) {
        VEDIT3_EDITOR_STATUS.is_own_lock_buffer_info = false;
    }

    error_code = pttui_thread_lock_unlock(LOCK_VEDIT3_BUFFER_INFO);

    return error_code;
}

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

/**
 * @brief
 * @details ref: detect_attr in edit.c
 *
 * @param ps [description]
 * @param len [description]
 * @param p_attr [description]
 */
Err
_vedit3_detect_attr(const char *ps, size_t len, int *p_attr)
{
    int attr = 0;
#ifdef PMORE_USE_ASCII_MOVIE
    if (mf_movieFrameHeader((unsigned char*)ps, (unsigned char*)ps + len)) attr |= EOATTR_MOVIECODE;
#endif

#ifdef USE_BBSLUA
    if (bbslua_isHeader(ps, ps + len))
    {
        attr |= EOATTR_BBSLUA;
        if (!curr_buf->synparser)
        {
            curr_buf->synparser = 1;
            // if you need indent, toggle by hotkey.
            // enabling indent by default may cause trouble to copy pasters
            // curr_buf->indent_mode = 1;
        }
    }
#endif
    return attr;
}

int _vedit3_syn_lua_keyword(const char *text, int n, char *wlen)
{
    int i = 0;
    const char * const *tbl = NULL;
    if (*text >= 'A' && *text <= 'Z')
    {
    // normal identifier
    while (n-- > 0 && (isalnum(*text) || *text == '_'))
    {
        text++;
        (*wlen) ++;
    }
    return 0;
    }
    if (*text >= '0' && *text <= '9')
    {
    // digits
    while (n-- > 0 && (isdigit(*text) || *text == '.' || *text == 'x'))
    {
        text++;
        (*wlen) ++;
    }
    return 5;
    }
    if (*text == '#')
    {
    text++;
    (*wlen) ++;
    // length of identifier
    while (n-- > 0 && (isalnum(*text) || *text == '_'))
    {
        text++;
        (*wlen) ++;
    }
    return -2;
    }

    // ignore non-identifiers
    if (!(*text >= 'a' && *text <= 'z'))
    return 0;

    // 1st, try keywords
    for (i = 0; luaKeywords[i] && *text >= *luaKeywords[i]; i++)
    {
    int l = strlen(luaKeywords[i]);
    if (n < l)
        continue;
    if (isalnum(text[l]))
        continue;
    if (strncmp(text, luaKeywords[i], l) == 0)
    {
        *wlen = l;
        return 3;
    }
    }
    for (i = 0; luaDataKeywords[i] && *text >= *luaDataKeywords[i]; i++)
    {
    int l = strlen(luaDataKeywords[i]);
    if (n < l)
        continue;
    if (isalnum(text[l]))
        continue;
    if (strncmp(text, luaDataKeywords[i], l) == 0)
    {
        *wlen = l;
        return 2;
    }
    }
    for (i = 0; luaFunctions[i] && *text >= *luaFunctions[i]; i++)
    {
    int l = strlen(luaFunctions[i]);
    if (n < l)
        continue;
    if (isalnum(text[l]))
        continue;
    if (strncmp(text, luaFunctions[i], l) == 0)
    {
        *wlen = l;
        return 6;
    }
    }
    for (i = 0; luaLibs[i]; i++)
    {
    int l = strlen(luaLibs[i]);
    if (n < l)
        continue;
    if (text[l] != '.' && text[l] != ':')
        continue;
    if (strncmp(text, luaLibs[i], l) == 0)
    {
        *wlen = l+1;
        text += l; text ++;
        n -= l; n--;
        break;
    }
    }

    tbl = luaLibAPI[i];
    if (!tbl)
    {
    // calcualte wlen
    while (n-- > 0 && (isalnum(*text) || *text == '_'))
    {
        text++;
        (*wlen) ++;
    }
    return 0;
    }

    for (i = 0; tbl[i]; i++)
    {
    int l = strlen(tbl[i]);
    if (n < l)
        continue;
    if (isalnum(text[l]))
        continue;
    if (strncmp(text, tbl[i], l) == 0)
    {
        *wlen += l;
        return 6;
    }
    }
    // luaLib. only
    return -6;
}
