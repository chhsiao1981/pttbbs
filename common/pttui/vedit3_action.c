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

        // ch as printable letter
        if (ch < 0x100 && isprint2(ch)) {
            /*
            error_code = VEDIT3_EDITOR_STATUS.is_phone && (pstr = phone_char(char)) ? _vedit3_action_insert_dchar(pstr) : _vedit3_action_insert_char(ch);
            if(error_code) break;
            */

            continue;
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
        /*
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
            */
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
    }
    if (error_code == S_ERR_NO_KEY) error_code = S_OK;

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

    if (!VEDIT3_EDITOR_STATUS.is_own_lock_buffer_info) return S_ERR_EDIT_LOCK;

    VEdit3Buffer *current_buffer = VEDIT3_EDITOR_STATUS.current_buffer;

    bool is_wordwrap = true;

    assert(VEDIT3_EDITOR_STATUS.current_col <= current_buffer->len);
//#ifdef DEBUG
    //assert(curr_buf->currline->mlength == WRAPMARGIN);
//#endif

    error_code = vedit3_wrlock_buffer_info();
    if (error_code) return error_code;

    // XXX remove block-cancel for now
    // block_cancel();


    int current_col_n2ansi = 0;
    if (VEDIT3_EDITOR_STATUS.current_col < current_buffer->len && !VEDIT3_EDITOR_STATUS.is_insert) {
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
#ifdef DEBUG
        assert(p->len < p->mlength);
#endif
        error_code = pttui_raw_shift_right(current_buffer->buf + VEDIT3_EDITOR_STATUS.current_col, current_buffer->len - VEDIT3_EDITOR_STATUS.current_col + 1);

        current_buffer->buf[VEDIT3_EDITOR_STATUS.current_col++] = ch;
        ++(current_buffer->len);
    }


    char *s = NULL;
    if (current_buffer->len >= WRAPMARGIN && !error_code) {
// find the last non-space word, pointing to the 0th char
        s = current_buffer->buf + current_buffer->len - 1;
        while (s != current_buffer->buf && *s == ' ') s--;
        while (s != current_buffer->buf && *s != ' ') s--;
        if (s == current_buffer) { // if only 1 word
            is_wordwrap = false;
            s = current_buffer->buf + (current_buffer->len - 2);
        }
    }

    VEdit3Buffer *new_buffer = NULL;
    if (!current_buffer->len >= WRAPMARGIN && !error_code) {
        error_code = vedit3_buffer_split(current_buffer, s - current_buffer->buf + 1, 0, &new_buffer);
    }

    if (!error_code && is_wordwrap && new_buffer && new_buffer->len >= 1) {
        VEDIT3_EDITOR_STATUS.current_buffer = new_buffer;
        current_buffer = new_buffer;

        if (current_buffer->buf[current_buffer->len - 1] != ' ') {
            current_buffer->buf[current_buffer->len] = ' ';
            current_buffer->buf[current_buffer->len + 1] = '\0';
            current_buffer->len++;
        }
    }

    Err error_code_lock = vedit3_wrunlock_buffer_info();
    if (error_code_lock) error_code = S_ERR_EDIT_LOCK;


//#ifdef DEBUG
//    assert(curr_buf->currline->mlength == WRAPMARGIN);
//#endif

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
    if (VEDIT3_EDITOR_STATUS.current_col < VEDIT3_EDITOR_STATUS.current_buffer->len) {
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

    VEDIT3_EDITOR_STATUS.current_col = VEDIT3_EDITOR_STATUS.current_buffer->len;

    return error_code;
}

Err
_vedit3_action_move_up()
{
    return S_OK;
}

Err
_vedit3_action_move_down()
{
    Err error_code = S_OK;
    // is end of file
    bool is_eof = false;
    error_code = pttui_is_eof(VEDIT3_EDITOR_STATUS.current_line, &VEDIT3_FILE_INFO, true, &is_eof);
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

    VEDIT3_EDITOR_STATUS.current_line++;
    VEDIT3_EDITOR_STATUS.current_col = VEDIT3_EDITOR_STATUS.current_col < VEDIT3_EDITOR_STATUS.current_buffer->len ? VEDIT3_EDITOR_STATUS.current_col : VEDIT3_EDITOR_STATUS.current_buffer->len;

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
    if (VEDIT3_EDITOR_STATUS.current_buffer->content_type != VEDIT3_STATE.end_line_content_type ||
        memcmp(VEDIT3_EDITOR_STATUS.current_buffer->the_id, VEDIT3_STATE.end_line_id) ||
        VEDIT3_EDITOR_STATUS.current_buffer->block_offset != VEDIT3_STATE.end_line_block_offset ||
        VEDIT3_EDITOR_STATUS.current_buffer->line_offset != VEDIT3_STATE.end_line_line_offset ||
        VEDIT3_EDITOR_STATUS.current_buffer->comment_offset != VEDIT3_STATE.end_line_comment_offset) return S_OK;




    return S_OK;
}

Err
_vedit3_action_move_end_line()
{
    VEDIT3_EDITOR_STATUS.current_col = VEDIT3_EDITOR_STATUS.current_buffer->len;

    return S_OK;
}

Err
_vedit3_action_move_begin_line()
{
    VEDIT3_EDITOR_STATUS.current_col = 0;

    return S_OK;
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
