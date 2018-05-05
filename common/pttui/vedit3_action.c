#include "cmpttui/vedit3_action.h"
#include "cmpttui/vedit3_action_private.h"

Err
vedit3_action_to_store(bool *is_end)
{
    Err error_code = S_OK;
    int ch;

    while (true) {
        error_code = _vedit3_action_get_key(&ch);
        if (error_code) break;

        switch(VEDIT3_EDITOR_STATUS.current_buffer->content_type) {
        case PTTDB_CONTENT_TYPE_MAIN:
            error_code = _vedit3_action_to_store_main(ch, is_end);
            break;
        case PTTDB_CONTENT_TYPE_COMMENT:
            error_code = _vedit3_action_to_store_comment(ch, is_end);
            break;
        case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
            error_code = _vedit3_action_to_store_comment_reply(ch, is_end);
            break;
        default:
            break;
        }
    }
    if (error_code == S_ERR_NO_KEY) error_code = S_OK;

    return error_code;
}


Err
_vedit3_action_to_store_main(int ch, bool *is_end) {
    Err error_code = S_OK;
    if (ch < 0x100 && isprint2(ch)) {
        error_code = _vedit3_action_insert_ch(ch);
        return error_code;
    }

    // ch as ctrl
    switch (ch) {
    case KEY_UP:
    case KEY_DOWN:
        break;
    case KEY_ESC:
        break;
    }


    // ctrl-command
    switch (ch) {
    case KEY_F10:
    case Ctrl('X'): // save and exit
        *is_end = true;
        break;
    case KEY_F5:    // goto-line
        break;

    case KEY_F8:    // users
        error_code = _vedit3_action_t_users();
        break;
    case Ctrl('W'):
        break;

    case Ctrl('Q'): // quit
        break;

    case Ctrl('C'): // ansi-code
        break;

    case KEY_ESC:   // escape
            break;

    case Ctrl('S'): // search-str
    case KEY_F3:
        break;

    /*
    case Ctrl('U'): // insert-esc
        error_code = _vedit3_action_insert_char(ESC_CHR);
        break;
    */
    case Ctrl('V'): // toggle ansi-color
        error_code = _vedit3_action_toggle_ansi();
        break;
    /*
    case Ctrl('I'): // insert-tab
        error_code = _vedit3_action_insert_tab();
        break;

    case KEY_ENTER: // new-line
        error_code = _vedit3_action_insert_new_line();
        break;

    case Ctrl('G'): // edit-assistant
        break;
    
    case Ctrl('P'): // toogle-phone-mode
        error_code = _vedit3_action_toggle_phone_mode();
        break;
    */
    case KEY_F1:
    case Ctrl('Z'): // help
        error_code = _vedit3_action_show_help();
        break;
    case Ctrl('L'): // redraw
        error_code = _vedit3_action_redraw();
        break;
    case KEY_LEFT:
        error_code = _vedit3_action_move_left();
        break;
    case KEY_RIGHT:
        error_code = _vedit3_action_move_right();
        break;
    case KEY_UP:
        error_code = _vedit3_action_move_up();
        break;
    case KEY_DOWN:
        error_code = _vedit3_action_move_down();
        break;
    case Ctrl('B'):
    case KEY_PGUP:
        error_code = _vedit3_action_move_pgup();
        break;
    case Ctrl('F'):
    case KEY_PGDN:
        error_code = _vedit3_action_move_pgdn();
        break;
    case KEY_END:
    case Ctrl('E'):
        error_code = _vedit3_action_move_end_line();
        break;
    case KEY_HOME:
    case Ctrl('A'):
        error_code = _vedit3_action_move_begin_line();
        break;
    /*
        case Ctrl(']'):
        error_code = _vedit3_action_move_start_file();
        break;
        case Ctrl('T'):
        error_code = _vedit3_action_move_tail_file();
        break;
        case Ctrl('O'):
        case KEY_INS:
        error_code = _vedit3_action_toggle_insert();
        break;
        case KEY_BS:
        error_code = _vedit3_action_backspace();
        break;
        case Ctrl('D'):
        case KEY_DEL:
        error_code = _vedit3_action_delete_char();
        break;
        case Ctrl('Y'):
        error_code = _vedit3_action_delete_line();
        break;
        case Ctrl('K'):
        error_code = _vedit3_action_delete_end_of_line();
        break;
    */
    default:
        break;
    }

    return error_code;
}

Err
_vedit3_action_to_store_comment(int ch, bool *is_end) {
    Err error_code = S_OK;
    // ctrl-command
    switch (ch) {
    case KEY_F10:
    case Ctrl('X'): // save and exit
        *is_end = true;
        break;
    case KEY_F5:    // goto-line
        break;
    case KEY_F8:    // users
        error_code = _vedit3_action_t_users();
        break;
    case Ctrl('W'):
        break;
    case Ctrl('Q'): // quit
        break;
    case Ctrl('C'): // ansi-code
        break;
    case Ctrl('S'): // search-str
    case KEY_F3:
        break;
    case Ctrl('V'): // toggle ansi-color
        error_code = _vedit3_action_toggle_ansi();
        break;
    /*
    case KEY_ENTER: // new-line
        error_code = _vedit3_action_insert_new_line();
        break;
    */
    case KEY_F1:
    case Ctrl('Z'): // help
        error_code = _vedit3_action_show_help();
        break;
    case Ctrl('L'): // redraw
        error_code = _vedit3_action_redraw();
        break;
    case KEY_UP:
        error_code = _vedit3_action_move_up();
        break;
    case KEY_DOWN:
        error_code = _vedit3_action_move_down();
        break;
    case Ctrl('B'):
    case KEY_PGUP:
        error_code = _vedit3_action_move_pgup();
        break;
    case Ctrl('F'):
    case KEY_PGDN:
        error_code = _vedit3_action_move_pgdn();
        break;
    /*
        case Ctrl(']'):
        error_code = _vedit3_action_move_start_file();
        break;
        case Ctrl('T'):
        error_code = _vedit3_action_move_tail_file();
        break;
        case Ctrl('O'):
        case KEY_INS:
        error_code = _vedit3_action_toggle_insert();
        break;
    */
    default:
        break;
    }

    return error_code;
}

Err
_vedit3_action_to_store_comment_reply(int ch, bool *is_end) {
    Err error_code = S_OK;
    if (ch < 0x100 && isprint2(ch)) {
        /*
        error_code = VEDIT3_EDITOR_STATUS.is_phone && (pstr = phone_char(char)) ? _vedit3_action_insert_dchar(pstr) : _vedit3_action_insert_char(ch);
        if(error_code) break;
        */

        return error_code;
    }

    // ch as ctrl
    switch (ch) {
    case KEY_UP:
    case KEY_DOWN:
        break;
    case KEY_ESC:
        break;
    }


    // ctrl-command
    switch (ch) {
    case KEY_F10:
    case Ctrl('X'): // save and exit
        *is_end = true;
        break;
    case KEY_F5:    // goto-line
        break;

    case KEY_F8:    // users
        error_code = _vedit3_action_t_users();
        break;
    case Ctrl('W'):
        break;

    case Ctrl('Q'): // quit
        break;

    case Ctrl('C'): // ansi-code
        break;

    case KEY_ESC:   // escape
            break;

    case Ctrl('S'): // search-str
    case KEY_F3:
        break;

    /*
    case Ctrl('U'): // insert-esc
        error_code = _vedit3_action_insert_char(ESC_CHR);
        break;
    */
    case Ctrl('V'): // toggle ansi-color
        error_code = _vedit3_action_toggle_ansi();
        break;
    /*
    case Ctrl('I'): // insert-tab
        error_code = _vedit3_action_insert_tab();
        break;

    case KEY_ENTER: // new-line
        error_code = _vedit3_action_insert_new_line();
        break;

    case Ctrl('G'): // edit-assistant
        break;
    
    case Ctrl('P'): // toogle-phone-mode
        error_code = _vedit3_action_toggle_phone_mode();
        break;
    */
    case KEY_F1:
    case Ctrl('Z'): // help
        error_code = _vedit3_action_show_help();
        break;
    case Ctrl('L'): // redraw
        error_code = _vedit3_action_redraw();
        break;
    case KEY_LEFT:
        error_code = _vedit3_action_move_left();
        break;
    case KEY_RIGHT:
        error_code = _vedit3_action_move_right();
        break;
    case KEY_UP:
        error_code = _vedit3_action_move_up();
        break;
    case KEY_DOWN:
        error_code = _vedit3_action_move_down();
        break;
    case Ctrl('B'):
    case KEY_PGUP:
        error_code = _vedit3_action_move_pgup();
        break;
    case Ctrl('F'):
    case KEY_PGDN:
        error_code = _vedit3_action_move_pgdn();
        break;
    case KEY_END:
    case Ctrl('E'):
        error_code = _vedit3_action_move_end_line();
        break;
    case KEY_HOME:
    case Ctrl('A'):
        error_code = _vedit3_action_move_begin_line();
        break;
    /*
        case Ctrl(']'):
        error_code = _vedit3_action_move_start_file();
        break;
        case Ctrl('T'):
        error_code = _vedit3_action_move_tail_file();
        break;
        case Ctrl('O'):
        case KEY_INS:
        error_code = _vedit3_action_toggle_insert();
        break;
        case KEY_BS:
        error_code = _vedit3_action_backspace();
        break;
        case Ctrl('D'):
        case KEY_DEL:
        error_code = _vedit3_action_delete_char();
        break;
        case Ctrl('Y'):
        error_code = _vedit3_action_delete_line();
        break;
        case Ctrl('K'):
        error_code = _vedit3_action_delete_end_of_line();
        break;
    */
    default:
        break;
    }

    return error_code;
}

/********************
 * action-command
 ********************/


/********************
 * action-util
 ********************/
/**
 * @brief [brief description]
 * @details refer: Ctrl('V') in vedit2 in edit.c
 */
Err
_vedit3_action_toggle_ansi()
{
    VEDIT3_EDITOR_STATUS.is_ansi ^= 1;

    // XXX block selection

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return S_OK;
}

/**
 * @brief [brief description]
 * @details ref: KEY_F8 in vedit2 in edit.c
 *
 */
Err
_vedit3_action_t_users()
{
    t_users();
    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return S_OK;
}

Err
_vedit3_action_get_key(int *ch)
{
    int is_available = vkey_poll(0);
    if (!is_available) return S_ERR_NO_KEY;

    int tmp = vkey();
    fprintf(stderr, "_vedit3_action_get_key: ch: %d\n", tmp);

    *ch = tmp;

    return S_OK;
}

Err
_vedit3_action_insert_ch(int ch)
{
    Err error_code = S_OK;
    char *pstr = NULL;
    if(VEDIT3_EDITOR_STATUS.is_phone) {
        error_code = pttui_phone_char(ch, VEDIT3_EDITOR_STATUS.phone_mode, &pstr);
    }   
    if(error_code) return error_code;

    error_code = vedit3_wrlock_buffer_info();

    if(pstr) {
        error_code = _vedit3_action_insert_dchar(pstr);
    }
    else {
        error_code = _vedit3_action_insert_char(ch);
    }

    Err error_code_lock = vedit3_wrunlock_buffer_info();
    if(!error_code && error_code_lock) error_code = S_ERR_EDIT_LOCK;

    return error_code;
}

Err
_vedit3_action_insert_dchar(const char *dchar)
{
    if (!VEDIT3_EDITOR_STATUS.is_own_wrlock_buffer_info) return S_ERR_EDIT_LOCK;

    Err error_code = _vedit3_action_insert_char(*dchar);
    Err error_code2 = _vedit3_action_insert_char(*(dchar+1));

    if(error_code) return error_code;
    if(error_code2) return error_code2;

    return S_OK;
}

/**
 * @brief
 * @details ref: insert_char in edit.c
 *          XXX Design decision: 1. Insert in the middle of the sentences.
 *                               2. ignore DBCSAWARE, need to be taken care of in other place.
 *
 *
 * @param ch [description]
 */

Err
_vedit3_action_insert_char(int ch)
{
    Err error_code = S_OK;

    if (!VEDIT3_EDITOR_STATUS.is_own_wrlock_buffer_info) return S_ERR_EDIT_LOCK;

    VEdit3Buffer *current_buffer = VEDIT3_EDITOR_STATUS.current_buffer;

    assert(VEDIT3_EDITOR_STATUS.current_col <= current_buffer->len_no_nl);
//#ifdef DEBUG
    //assert(curr_buf->currline->mlength == WRAPMARGIN);
//#endif

    // XXX remove block-cancel for now
    // block_cancel();


    int current_col_n2ansi = 0;
    if (VEDIT3_EDITOR_STATUS.current_col < current_buffer->len_no_nl && !VEDIT3_EDITOR_STATUS.is_insert) {
        // no need to change len
        current_buffer->buf[VEDIT3_EDITOR_STATUS.current_col++] = ch;

        /* Thor: ansi 編輯, 可以overwrite, 不蓋到 ansi code */
        if (VEDIT3_EDITOR_STATUS.is_ansi && !error_code) {
            error_code = pttui_n2ansi(VEDIT3_EDITOR_STATUS.current_col, current_buffer->buf, &current_col_n2ansi);
        }

        if (VEDIT3_EDITOR_STATUS.is_ansi && !error_code) {
            error_code = pttui_ansi2n(current_col_n2ansi, current_buffer->buf, &VEDIT3_EDITOR_STATUS.current_col);
        }
    }
    else { // insert-mode
        error_code = pttui_raw_shift_right(current_buffer->buf + VEDIT3_EDITOR_STATUS.current_col, current_buffer->len_no_nl - VEDIT3_EDITOR_STATUS.current_col + 1);

        current_buffer->buf[VEDIT3_EDITOR_STATUS.current_col++] = ch;
        ++(current_buffer->len);
        ++(current_buffer->len_no_nl);
        current_buffer->buf[current_buffer->len_no_nl] = 0;
    }
    current_buffer->is_modified = true;

    error_code = _vedit3_action_ensure_buffer_wrap();

//#ifdef DEBUG
//    assert(curr_buf->currline->mlength == WRAPMARGIN);
//#endif

    return error_code;
}

Err
_vedit3_action_ensure_buffer_wrap()
{
    Err error_code = S_OK;
    VEdit3Buffer *current_buffer = VEDIT3_EDITOR_STATUS.current_buffer;

    fprintf(stderr, "vedit3_action._vedit3_action_ensure_buffer_wrap: len_no_nl: %d WRAPMARGIN: %d\n", current_buffer->len_no_nl, WRAPMARGIN);
    if (current_buffer->len_no_nl < WRAPMARGIN) {
        return S_OK;
    }

    if (!VEDIT3_EDITOR_STATUS.is_own_wrlock_buffer_info) return S_ERR_EDIT_LOCK;

    fprintf(stderr, "vedit3_action._vedit3_action_ensure_buffer_wrap: buf: %s len_no_nl: %d\n", current_buffer->buf, current_buffer->len_no_nl);

    bool is_wordwrap = true;
    char *s = current_buffer->buf + current_buffer->len_no_nl - 1;
    while (s != current_buffer->buf && *s == ' ') s--;
    while (s != current_buffer->buf && *s != ' ') s--;
    if (s == current_buffer->buf) { // if only 1 word
        is_wordwrap = false;
        s = current_buffer->buf + (current_buffer->len_no_nl - 2);
    }

    VEdit3Buffer *new_buffer = NULL;

    fprintf(stderr, "vedit3_action._vedit3_action_ensure_buffer_wrap: to buffer_split: offset-s: %d s: %d\n", s - current_buffer->buf, s[0]);
    error_code = _vedit3_action_buffer_split(current_buffer, s - current_buffer->buf + 1, 0, &new_buffer);
    fprintf(stderr, "vedit3_action._vedit3_action_ensure_buffer_wrap: after buffer_split\n");

    int new_buffer_len_no_nl = new_buffer ? new_buffer->len_no_nl : 0;
    if (!error_code && is_wordwrap && new_buffer && new_buffer_len_no_nl >= 1) {
        if (new_buffer->buf[new_buffer_len_no_nl - 1] != ' ') {
            new_buffer->buf[new_buffer_len_no_nl] = ' ';
            new_buffer->len++;
            new_buffer->len_no_nl++;
            new_buffer->buf[new_buffer_len_no_nl] = '\0';
        }
    }

    error_code = _vedit3_action_ensure_current_col(VEDIT3_EDITOR_STATUS.current_col);

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return error_code;
}

Err
_vedit3_action_ensure_current_col(int current_col) {
    VEDIT3_EDITOR_STATUS.current_col = (current_col < VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl || VEDIT3_EDITOR_STATUS.current_buffer->content_type == PTTDB_CONTENT_TYPE_COMMENT) ? current_col : VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl;

    return S_OK;
}

Err
_vedit3_action_buffer_split(VEdit3Buffer *current_buffer, int pos, int indent, VEdit3Buffer **new_buffer)
{
    Err error_code = S_OK;

    // XXX should not happen.
    if(pos > current_buffer->len_no_nl) return S_OK;

    *new_buffer = malloc(sizeof(VEdit3Buffer));
    VEdit3Buffer *p_new_buffer = *new_buffer;
    bzero(p_new_buffer, sizeof(VEdit3Buffer));

    memcpy(p_new_buffer->the_id, current_buffer->the_id, UUIDLEN);
    p_new_buffer->content_type = current_buffer->content_type;
    p_new_buffer->block_offset = current_buffer->block_offset;
    p_new_buffer->line_offset = current_buffer->line_offset + 1;
    p_new_buffer->comment_offset = current_buffer->comment_offset;
    p_new_buffer->load_line_offset = -1;

    p_new_buffer->storage_type = PTTDB_STORAGE_TYPE_OTHER;

    p_new_buffer->is_modified = true;
    p_new_buffer->is_new = true;

    p_new_buffer->len_no_nl = current_buffer->len_no_nl - pos + indent;
    p_new_buffer->buf = malloc(MAX_TEXTLINE_SIZE + 1);
    memset(p_new_buffer->buf, ' ', indent);

    char *p_buf = current_buffer->buf + pos;
    // XXX indent-mode
    /*
    if (curr_buf->indent_mode) {
        ptr = next_non_space_char(ptr);
        p->len = strlen(ptr) + spcs;
    }
    */
    memcpy(p_new_buffer->buf + indent, p_buf, current_buffer->len_no_nl - pos);
    p_new_buffer->buf[p_new_buffer->len_no_nl] = 0;
    p_new_buffer->len = p_new_buffer->len_no_nl + 1;

    current_buffer->len += (pos - current_buffer->len_no_nl);
    current_buffer->len_no_nl = pos;
    current_buffer->buf[current_buffer->len_no_nl] = 0;

    error_code = vedit3_buffer_insert_buffer(current_buffer, p_new_buffer, &VEDIT3_BUFFER_INFO);

    // buffer after new_buffer
    for(VEdit3Buffer *p_buffer2 = p_new_buffer->next; p_buffer2 && p_buffer2->content_type == p_new_buffer->content_type && p_buffer2->block_offset == p_new_buffer->block_offset && p_buffer2->comment_offset == p_new_buffer->comment_offset; p_buffer2->line_offset++, p_buffer2 = p_buffer2->next);

    // file-info
    error_code = vedit3_wrlock_file_info();
    if(error_code) return error_code;

    switch(current_buffer->content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        VEDIT3_FILE_INFO.main_blocks[current_buffer->block_offset].n_line++;
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        VEDIT3_FILE_INFO.comments[current_buffer->comment_offset].comment_reply_blocks[current_buffer->block_offset].n_line++;
        break;
    default:
        break;
    }

    Err error_code_lock = vedit3_wrunlock_file_info();
    if(!error_code && error_code_lock) error_code = S_ERR_EDIT_LOCK;

    return error_code;
}

Err
_vedit3_action_move_right()
{
    Err error_code = S_OK;
    int ansi_current_col = 0;
    int mbcs_current_col = 0;
    int orig_current_col = VEDIT3_EDITOR_STATUS.current_col;
    // within the same line
    if (VEDIT3_EDITOR_STATUS.current_col < VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl) {
        // TODO use function-map to replace if-else
        if (VEDIT3_EDITOR_STATUS.is_ansi) {
            error_code = pttui_n2ansi(VEDIT3_EDITOR_STATUS.current_col, VEDIT3_EDITOR_STATUS.current_buffer->buf, &ansi_current_col);
            ansi_current_col++;
            error_code = pttui_ansi2n(ansi_current_col, VEDIT3_EDITOR_STATUS.current_buffer->buf, &VEDIT3_EDITOR_STATUS.current_col);


        }
        else {
            VEDIT3_EDITOR_STATUS.current_col++;
        }

        if (VEDIT3_EDITOR_STATUS.is_mbcs) {
            error_code = pttui_fix_cursor(VEDIT3_EDITOR_STATUS.current_buffer->buf, VEDIT3_EDITOR_STATUS.current_col, PTTUI_FIX_CURSOR_DIR_RIGHT, &mbcs_current_col);
            VEDIT3_EDITOR_STATUS.current_col = mbcs_current_col;
        }

        fprintf(stderr, "vedit3_action._vedit3_action_move_right: within same line: orig_current_col: %d is_ansi: %d is_mbcs: %d new_current_col: %d\n", orig_current_col, VEDIT3_EDITOR_STATUS.is_ansi, VEDIT3_EDITOR_STATUS.is_mbcs, VEDIT3_EDITOR_STATUS.current_col);

        return S_OK;
    }

    // need to move to next line

    // move to next line
    error_code = _vedit3_action_move_down();

    VEDIT3_EDITOR_STATUS.current_col = 0;

    return error_code;
}

Err
_vedit3_action_move_left()
{
    Err error_code = S_OK;
    int ansi_current_col = 0;
    int mbcs_current_col = 0;
    int orig_current_col = VEDIT3_EDITOR_STATUS.current_col;
    // within the same line
    if (VEDIT3_EDITOR_STATUS.current_col) {
        // TODO use function-map to replace if-else
        if (VEDIT3_EDITOR_STATUS.is_ansi) {
            error_code = pttui_n2ansi(VEDIT3_EDITOR_STATUS.current_col, VEDIT3_EDITOR_STATUS.current_buffer->buf, &ansi_current_col);
            ansi_current_col--;
            error_code = pttui_ansi2n(ansi_current_col, VEDIT3_EDITOR_STATUS.current_buffer->buf, &VEDIT3_EDITOR_STATUS.current_col);


        }
        else {
            VEDIT3_EDITOR_STATUS.current_col--;
        }

        if (VEDIT3_EDITOR_STATUS.is_mbcs) {
            error_code = pttui_fix_cursor(VEDIT3_EDITOR_STATUS.current_buffer->buf, VEDIT3_EDITOR_STATUS.current_col, PTTUI_FIX_CURSOR_DIR_LEFT, &mbcs_current_col);
            VEDIT3_EDITOR_STATUS.current_col = mbcs_current_col;
        }

        fprintf(stderr, "vedit3_action._vedit3_action_move_left: within same line: orig_current_col: %d is_ansi: %d is_mbcs: %d new_current_col: %d\n", orig_current_col, VEDIT3_EDITOR_STATUS.is_ansi, VEDIT3_EDITOR_STATUS.is_mbcs, VEDIT3_EDITOR_STATUS.current_col);

        return S_OK;
    }

    // is beginning of file
    if (VEDIT3_EDITOR_STATUS.current_line == 0 && VEDIT3_EDITOR_STATUS.current_col == 0) return S_OK;

    // move to previous line
    error_code = _vedit3_action_move_up();

    VEDIT3_EDITOR_STATUS.current_col = VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl;

    return error_code;
}

Err
_vedit3_action_move_up()
{
    Err error_code = S_OK;
    // is begin of file
    bool is_begin = false;
    error_code = vedit3_buffer_is_begin_of_file(VEDIT3_EDITOR_STATUS.current_buffer, &VEDIT3_FILE_INFO, &is_begin);
    if (error_code) return error_code;
    if (is_begin) return S_OK;

    // check begin-of-window
    error_code = _vedit3_action_move_up_ensure_top_of_window();
    if (error_code) return error_code;

    // move next
    error_code = vedit3_lock_buffer_info();
    if (error_code) return error_code;

    if (VEDIT3_EDITOR_STATUS.current_buffer->pre) VEDIT3_EDITOR_STATUS.current_buffer = VEDIT3_EDITOR_STATUS.current_buffer->pre;

    Err error_code_lock = vedit3_unlock_buffer_info();
    if (error_code_lock) error_code = S_ERR_EDIT_LOCK;

    if (VEDIT3_EDITOR_STATUS.current_line > 0) VEDIT3_EDITOR_STATUS.current_line--;

    VEDIT3_EDITOR_STATUS.current_buffer_line--;

    error_code = _vedit3_action_ensure_current_col(VEDIT3_EDITOR_STATUS.current_col);

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return error_code;
}

/**
 * @brief [brief description]
 * @details given that current-buffer is not eof, want to check the end of window and do corresponding works to maintain the state
 */
Err
_vedit3_action_move_up_ensure_top_of_window()
{
    // XXX assuming current-buffer <= end-line-buffer
    // need to move down
    if (VEDIT3_EDITOR_STATUS.current_line != 0) return S_OK;

    UUID main_id = {};
    memcpy(main_id, VEDIT3_STATE.main_id, UUIDLEN);
    int n_window_line = VEDIT3_STATE.n_window_line;

    UUID new_id = {};
    enum PttDBContentType new_content_type = PTTDB_CONTENT_TYPE_MAIN;
    int new_block_offset = 0;
    int new_line_offset = 0;
    int new_comment_offset = 0;
    enum StorageType _dummy = PTTDB_STORAGE_TYPE_MONGO;

    Err error_code = file_info_get_pre_line(&VEDIT3_FILE_INFO, VEDIT3_STATE.top_line_id, VEDIT3_STATE.top_line_content_type, VEDIT3_STATE.top_line_block_offset, VEDIT3_STATE.top_line_line_offset, VEDIT3_STATE.top_line_comment_offset, new_id, &new_content_type, &new_block_offset, &new_line_offset, &new_comment_offset, &_dummy);
    if (error_code) return error_code;

    fprintf(stderr, "vedit3_action._vedit3_action_move_up_ensure_top_of_window: orig_content_type: %d orig_block_offset: %d orig_line_offset: %d orig_comment_offset: %d new_content_type: %d new_block_offset: %d new_line_offset: %d new_comment_offset: %d\n", VEDIT3_STATE.top_line_content_type, VEDIT3_STATE.top_line_block_offset, VEDIT3_STATE.top_line_line_offset, VEDIT3_STATE.top_line_comment_offset, new_content_type, new_block_offset, new_line_offset, new_comment_offset);

    error_code = vedit3_set_expected_state(main_id, new_content_type, new_id, new_block_offset, new_line_offset, new_comment_offset, n_window_line);
    if (error_code) return error_code;

    fprintf(stderr, "vedit3_action._vedit3_action_move_down_ensure_end_of_window: to wait buffer state sync\n");

    error_code = vedit3_wait_buffer_state_sync(DEFAULT_ITER_VEDIT3_WAIT_BUFFER_STATE_SYNC);
    if (error_code) return error_code;

    return error_code;
}

Err
_vedit3_action_move_down()
{
    Err error_code = S_OK;
    // is end of file
    bool is_eof = false;
    error_code = vedit3_buffer_is_eof(VEDIT3_EDITOR_STATUS.current_buffer, &VEDIT3_FILE_INFO, &is_eof);
    if (error_code) return error_code;
    if (is_eof) return S_OK;

    // check end-of-window
    error_code = _vedit3_action_move_down_ensure_end_of_window();
    if (error_code) return error_code;

    // move next
    error_code = vedit3_lock_buffer_info();
    if (error_code) return error_code;

    if (VEDIT3_EDITOR_STATUS.current_buffer->next) VEDIT3_EDITOR_STATUS.current_buffer = VEDIT3_EDITOR_STATUS.current_buffer->next;

    Err error_code_lock = vedit3_unlock_buffer_info();
    if (error_code_lock) error_code = S_ERR_EDIT_LOCK;

    if (VEDIT3_EDITOR_STATUS.current_line < b_lines - 1) VEDIT3_EDITOR_STATUS.current_line++;

    VEDIT3_EDITOR_STATUS.current_buffer_line++;

    error_code = _vedit3_action_ensure_current_col(VEDIT3_EDITOR_STATUS.current_col);

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return error_code;
}

/**
 * @brief [brief description]
 * @details given that current-buffer is not eof, want to check the end of window and do corresponding works to maintain the state
 */
Err
_vedit3_action_move_down_ensure_end_of_window()
{
    // XXX assuming current-buffer <= end-line-buffer
    // need to move down
    if (VEDIT3_EDITOR_STATUS.current_line != b_lines - 1) return S_OK;

    UUID main_id = {};
    memcpy(main_id, VEDIT3_STATE.main_id, UUIDLEN);
    int n_window_line = VEDIT3_STATE.n_window_line;

    UUID new_id = {};
    enum PttDBContentType new_content_type = PTTDB_CONTENT_TYPE_MAIN;
    int new_block_offset = 0;
    int new_line_offset = 0;
    int new_comment_offset = 0;
    enum StorageType _dummy = PTTDB_STORAGE_TYPE_MONGO;

    Err error_code = file_info_get_next_line(&VEDIT3_FILE_INFO, VEDIT3_STATE.top_line_id, VEDIT3_STATE.top_line_content_type, VEDIT3_STATE.top_line_block_offset, VEDIT3_STATE.top_line_line_offset, VEDIT3_STATE.top_line_comment_offset, new_id, &new_content_type, &new_block_offset, &new_line_offset, &new_comment_offset, &_dummy);
    if (error_code) return error_code;

    fprintf(stderr, "vedit3_action._vedit3_action_move_down_ensure_end_of_window: orig_content_type: %d orig_block_offset: %d orig_line_offset: %d orig_comment_offset: %d new_content_type: %d new_block_offset: %d new_line_offset: %d new_comment_offset: %d\n", VEDIT3_STATE.top_line_content_type, VEDIT3_STATE.top_line_block_offset, VEDIT3_STATE.top_line_line_offset, VEDIT3_STATE.top_line_comment_offset, new_content_type, new_block_offset, new_line_offset, new_comment_offset);

    error_code = vedit3_set_expected_state(main_id, new_content_type, new_id, new_block_offset, new_line_offset, new_comment_offset, n_window_line);
    if (error_code) return error_code;

    fprintf(stderr, "vedit3_action._vedit3_action_move_down_ensure_end_of_window: to wait buffer state sync\n");

    error_code = vedit3_wait_buffer_state_sync(DEFAULT_ITER_VEDIT3_WAIT_BUFFER_STATE_SYNC);
    if (error_code) return error_code;

    return error_code;
}

Err
_vedit3_action_move_end_line()
{
    VEDIT3_EDITOR_STATUS.current_col = VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl;

    return S_OK;
}

Err
_vedit3_action_move_begin_line()
{
    VEDIT3_EDITOR_STATUS.current_col = 0;

    return S_OK;
}

Err
_vedit3_action_move_pgup()
{
    Err error_code = S_OK;
    bool is_begin = false;
    int current_line = VEDIT3_EDITOR_STATUS.current_line;
    int current_col = VEDIT3_EDITOR_STATUS.current_col;
    for(int i = 0; i < b_lines - 1; i++) {
        error_code = _vedit3_action_move_up();
        if(error_code) break;
        error_code = vedit3_buffer_is_begin_of_file(VEDIT3_EDITOR_STATUS.current_buffer, &VEDIT3_FILE_INFO, &is_begin);
        if(error_code) break;
        if(is_begin) break;
    }
    if(error_code) return error_code;

    if(!is_begin) {
        for(int i = 0; i < current_line; i++) {
            error_code = _vedit3_action_move_up();
            if(error_code) break;
        }
        if(error_code) return error_code;

        for(int i = 0; i < current_line; i++) {
            error_code = _vedit3_action_move_down();
            if(error_code) break;
        }
        if(error_code) return error_code;
    }

    error_code = _vedit3_action_ensure_current_col(current_col);

    return error_code;
}

Err
_vedit3_action_move_pgdn()
{
    Err error_code = S_OK;
    bool is_eof = false;
    int current_line = VEDIT3_EDITOR_STATUS.current_line;
    int current_col = VEDIT3_EDITOR_STATUS.current_col;
    for(int i = 0; i < b_lines - 1; i++) {
        error_code = _vedit3_action_move_down();
        if(error_code) break;
        error_code = vedit3_buffer_is_eof(VEDIT3_EDITOR_STATUS.current_buffer, &VEDIT3_FILE_INFO, &is_eof);
        if(error_code) break;
        if(is_eof) break;
    }
    if(error_code) return error_code;

    if(!is_eof) {
        for(int i = b_lines - 1; i > current_line; i--) {
            error_code = _vedit3_action_move_down();
            if(error_code) break;
        }
        if(error_code) return error_code;

        for(int i = b_lines - 1; i > current_line; i--) {
            error_code = _vedit3_action_move_up();
            if(error_code) break;
        }
        if(error_code) return error_code;
    }

    error_code = _vedit3_action_ensure_current_col(current_col);

    return error_code;
}

Err
_vedit3_action_redraw()
{
    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return S_OK;
}

Err
_vedit3_action_show_help()
{
    more("etc/ve.hlp", true);

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return S_OK;
}
