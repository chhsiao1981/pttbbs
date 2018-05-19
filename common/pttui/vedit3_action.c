#include "cmpttui/vedit3_action.h"
#include "cmpttui/vedit3_action_private.h"

Err
vedit3_action_to_store(bool *is_end)
{
    Err error_code = S_OK;
    int ch;

    error_code = _vedit3_action_get_key(&ch);

    if (error_code) return error_code;

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

    return error_code;
}


Err
_vedit3_action_to_store_main(int ch, bool *is_end) {
    Err error_code = S_OK;
    if (ch < 0x100 && isprint2(ch)) {
        error_code = vedit3_action_insert_char(ch);
        return error_code;
    }

    // XXX remove lua-parser for now
    // if(ch == KEY_ESC && KEY_ESC_arg == 'S') VEDIT3_EDITOR_STATUS.is_syntax_parser = !VEDIT3_EDITOR_STATUS.is_syntax_parser;

    // ch as ctrl
    if(ch == KEY_ESC) ch = pttui_ctrl_key_ne(ch);

    // ctrl-command
    switch (ch) {
    case KEY_F10:
    case Ctrl('X'): // save and exit
        error_code = vedit3_action_save_and_exit(is_end);
        break;
    case KEY_F5:    // goto-line
        error_code = vedit3_action_goto();
        break;
    case KEY_F8:    // users
        error_code = vedit3_action_t_users();
        break;
    case Ctrl('W'):
        error_code = vedit3_action_block_cut();
        break;
    case Ctrl('Q'): // quit
        error_code = vedit3_action_exit(is_end);
        break;
    case Ctrl('C'): // ansi-code
        error_code = vedit3_action_insert_ansi_code();
        break;
    case KEY_ESC: // escape
        error_code = vedit3_action_escape();
        break;
    case Ctrl('S'): // search-str
    case KEY_F3:
        error_code = vedit3_action_search();
        break;
    case Ctrl('U'): // insert-esc
        error_code = vedit3_action_insert_char(ESC_CHR);
        break;
    case Ctrl('V'): // toggle ansi-color
        error_code = vedit3_action_toggle_ansi();
        break;
    case Ctrl('I'): // insert-tab
        error_code = vedit3_action_insert_tab();
        break;
    case KEY_ENTER: // new-line
        error_code = vedit3_action_insert_new_line();
        break;
    case Ctrl('G'): // edit-assistant
        error_code = vedit3_action_edit_assist();
        break;
    case Ctrl('P'): // toogle-phone-mode
        error_code = vedit3_action_toggle_phone_mode();
        break;
    case KEY_F1:
    case Ctrl('Z'): // help
        error_code = vedit3_action_show_help();
        break;
    case Ctrl('L'): // redraw
        error_code = vedit3_action_redraw();
        break;
    case KEY_LEFT:
        error_code = vedit3_action_move_left();
        break;
    case KEY_RIGHT:
        error_code = vedit3_action_move_right();
        break;
    case KEY_UP:
        error_code = vedit3_action_move_up();
        break;
    case KEY_DOWN:
        error_code = vedit3_action_move_down();
        break;
    case Ctrl('B'):
    case KEY_PGUP:
        error_code = vedit3_action_move_pgup();
        break;
    case Ctrl('F'):
    case KEY_PGDN:
        error_code = vedit3_action_move_pgdn();
        break;
    case KEY_END:
    case Ctrl('E'):
        error_code = vedit3_action_move_end_line();
        break;
    case KEY_HOME:
    case Ctrl('A'):
        error_code = vedit3_action_move_begin_line();
        break;
    case Ctrl(']'):
        error_code = vedit3_action_move_start_file();
        break;
    case Ctrl('T'):
        error_code = vedit3_action_move_tail_file();
        break;
    case Ctrl('O'):
        case KEY_INS:
        error_code = vedit3_action_toggle_insert();
        break;
    case KEY_BS:
        error_code = vedit3_action_backspace();
        break;
    case Ctrl('D'):
    case KEY_DEL:
        error_code = vedit3_action_delete_char();
        break;
    case Ctrl('Y'):
        error_code = vedit3_action_delete_line();
        break;
    case Ctrl('K'):
        error_code = vedit3_action_delete_end_of_line();
        break;
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
        error_code = vedit3_action_save_and_exit(is_end);
        break;
    case KEY_F5:    // goto-line
        error_code = vedit3_action_goto();
        break;
    case KEY_F8:    // users
        error_code = vedit3_action_t_users();
        break;
    case Ctrl('W'):
        break;
    case Ctrl('Q'): // quit
        error_code = vedit3_action_exit(is_end);
        break;
    case Ctrl('C'): // ansi-code
        break;
    case Ctrl('S'): // search-str
    case KEY_F3:
        error_code = vedit3_action_search();
        break;
    case Ctrl('V'): // toggle ansi-color
        error_code = vedit3_action_toggle_ansi();
        break;
    case KEY_F1:
    case Ctrl('Z'): // help
        error_code = vedit3_action_show_help();
        break;
    case Ctrl('L'): // redraw
        error_code = vedit3_action_redraw();
        break;
    case KEY_LEFT:
    case KEY_UP:
        error_code = vedit3_action_move_up();
        break;
    case KEY_RIGHT:
    case KEY_DOWN:
        error_code = vedit3_action_move_down();
        break;
    case Ctrl('B'):
    case KEY_PGUP:
        error_code = vedit3_action_move_pgup();
        break;
    case Ctrl('F'):
    case KEY_PGDN:
        error_code = vedit3_action_move_pgdn();
        break;
    case Ctrl(']'):
        error_code = vedit3_action_move_start_file();
        break;
    case Ctrl('T'):
        error_code = vedit3_action_move_tail_file();
        break;
    case Ctrl('O'):
    case KEY_INS:
        error_code = vedit3_action_toggle_insert();
        break;
    case KEY_ENTER:
    case Ctrl('R'):
        error_code = vedit3_action_comment_init_comment_reply();
        break;
    case Ctrl('D'):
    case KEY_DEL:
    case Ctrl('Y'):
    case Ctrl('K'):
        error_code = vedit3_action_comment_delete_comment();
        break;
    default:
        break;
    }

    return error_code;
}

Err
_vedit3_action_to_store_comment_reply(int ch, bool *is_end) {
    Err error_code = S_OK;
    if (ch < 0x100 && isprint2(ch)) {
        error_code = vedit3_action_insert_char(ch);
        return error_code;
    }

    // ch as ctrl
    if(ch == KEY_ESC) ch = pttui_ctrl_key_ne(ch);

    // ctrl-command
    switch (ch) {
    case KEY_F10:
    case Ctrl('X'): // save and exit
        error_code = vedit3_action_save_and_exit(is_end);
        break;
    case KEY_F5:    // goto-line
        error_code = vedit3_action_goto();
        break;
    case KEY_F8:    // users
        error_code = vedit3_action_t_users();
        break;
    case Ctrl('W'):
        error_code = vedit3_action_block_cut();
        break;
    case Ctrl('Q'): // quit
        error_code = vedit3_action_exit(is_end);
        break;
    case Ctrl('C'): // ansi-code
        error_code = vedit3_action_insert_ansi_code();
        break;
    case KEY_ESC:   // escape
        error_code = vedit3_action_escape();
        break;
    case Ctrl('S'): // search-str
    case KEY_F3:
        error_code = vedit3_action_search();
        break;
    case Ctrl('U'): // insert-esc
        error_code = vedit3_action_insert_char(ESC_CHR);
        break;
    case Ctrl('V'): // toggle ansi-color
        error_code = vedit3_action_toggle_ansi();
        break;
    case Ctrl('I'): // insert-tab
        error_code = vedit3_action_insert_tab();
        break;
    case KEY_ENTER: // new-line
        error_code = vedit3_action_insert_new_line();
        break;
    case Ctrl('G'): // edit-assistant
        break;
    case Ctrl('P'): // toogle-phone-mode
        error_code = vedit3_action_toggle_phone_mode();
        break;
    case KEY_F1:
    case Ctrl('Z'): // help
        error_code = vedit3_action_show_help();
        break;
    case Ctrl('L'): // redraw
        error_code = vedit3_action_redraw();
        break;
    case KEY_LEFT:
        error_code = vedit3_action_move_left();
        break;
    case KEY_RIGHT:
        error_code = vedit3_action_move_right();
        break;
    case KEY_UP:
        error_code = vedit3_action_move_up();
        break;
    case KEY_DOWN:
        error_code = vedit3_action_move_down();
        break;
    case Ctrl('B'):
    case KEY_PGUP:
        error_code = vedit3_action_move_pgup();
        break;
    case Ctrl('F'):
    case KEY_PGDN:
        error_code = vedit3_action_move_pgdn();
        break;
    case KEY_END:
    case Ctrl('E'):
        error_code = vedit3_action_move_end_line();
        break;
    case KEY_HOME:
    case Ctrl('A'):
        error_code = vedit3_action_move_begin_line();
        break;
    case Ctrl(']'):
        error_code = vedit3_action_move_start_file();
        break;
    case Ctrl('T'):
        error_code = vedit3_action_move_tail_file();
        break;
    case Ctrl('O'):
        case KEY_INS:
        error_code = vedit3_action_toggle_insert();
        break;
    case KEY_BS:
        error_code = vedit3_action_backspace();
        break;
    case Ctrl('D'):
    case KEY_DEL:
        error_code = vedit3_action_delete_char();
        break;
    case Ctrl('Y'):
        error_code = vedit3_action_delete_line();
        break;
    case Ctrl('K'):
        error_code = vedit3_action_delete_end_of_line();
        break;
    default:
        break;
    }

    return error_code;
}

Err
_vedit3_action_get_key(int *ch)
{
    int is_available = vkey_poll(0);
    if (!is_available) return S_ERR_NO_KEY;

    int tmp = vkey();

    *ch = tmp;

    return S_OK;
}


/********************
 * action-command
 ********************/

Err
vedit3_action_save_and_exit(bool *is_end)
{
    Err error_code = S_OK;

    PTTUI_BUFFER_INFO.is_to_save = true;

    bool tmp_is_end = false;
    struct timespec req = {0, NS_SLEEP_WAIT_BUFFER_SAVE};
    struct timespec rem = {};

    int ret_sleep = 0;
    int i = 0;
    for(i = 0; i < N_ITER_WAIT_BUFFER_SAVE; i++) {
        if(PTTUI_BUFFER_INFO.is_saved) tmp_is_end = true;

        if(tmp_is_end) break;

        ret_sleep = nanosleep(&req, &rem);
        if(ret_sleep) {
            error_code = S_ERR;
            break;
        }
    }

    if(i == N_ITER_WAIT_BUFFER_SAVE) error_code = S_ERR_SAVE;

    *is_end = true;

    return error_code;
}

Err
vedit3_action_goto()
{
    return S_OK;
}

Err
vedit3_action_block_cut()
{
    return S_OK;
}

Err
vedit3_action_exit(bool *is_end)
{
    *is_end = true;

    return S_OK;
}

Err
vedit3_action_insert_ansi_code()
{
    return S_OK;
}

Err
vedit3_action_escape()
{
    return S_OK;
}

Err
vedit3_action_search()
{
    return S_OK;
}

Err
vedit3_action_toggle_insert()
{
    return S_OK;
}

Err
vedit3_action_insert_tab()
{
    return S_OK;
}

Err
vedit3_action_edit_assist()
{
    return S_OK;
}

Err
vedit3_action_toggle_phone_mode()
{
    return S_OK;
}

Err
vedit3_action_comment_init_comment_reply()
{
    Err error_code = S_OK;

    int comment_offset = VEDIT3_EDITOR_STATUS.current_buffer->comment_offset;

    CommentInfo *p_comment = PTTUI_FILE_INFO.comments + comment_offset;

    if(p_comment->n_comment_reply_total_line) {
        return vedit3_action_move_down();
    }

    bool is_lock_file_info = false;
    bool is_lock_wr_buffer_info = false;
    bool is_lock_buffer_info = false;

    error_code = vedit3_repl_wrlock_file_info_buffer_info(&is_lock_file_info, &is_lock_wr_buffer_info, &is_lock_buffer_info);

    ContentBlockInfo *p_comment_reply = NULL;
    unsigned char zero_comment_reply_id = 0;

    // file-info
    if(!error_code) {
        // XXX hack for the temporary comment-reply-id
        if(!p_comment->n_comment_reply_block) {
            memcpy(p_comment->comment_reply_id, p_comment->comment_id, UUIDLEN);
            zero_comment_reply_id = p_comment->comment_id[0];
            p_comment->comment_reply_id[0] = zero_comment_reply_id == 255 ? 0 : zero_comment_reply_id + 1;

            p_comment->n_comment_reply_block = 1;
            p_comment->comment_reply_blocks = malloc(sizeof(ContentBlockInfo));
            bzero(p_comment->comment_reply_blocks, sizeof(ContentBlockInfo));
        }

        p_comment->n_comment_reply_total_line = 1;

        p_comment_reply = p_comment->comment_reply_blocks;

        if(!p_comment_reply->n_file) {
            p_comment_reply->n_file = 1;
            p_comment_reply->file_n_line = malloc(sizeof(int));
        }

        p_comment_reply->storage_type = PTTDB_STORAGE_TYPE_FILE;

        p_comment_reply->file_n_line[0] = 1;

        p_comment_reply->n_line = 1;
        p_comment_reply->n_line_in_db = 1;

        PTTUI_FILE_INFO.n_total_line++;
    }

    // file
    if(!error_code) {
        error_code = pttdb_file_save_data(PTTUI_FILE_INFO.main_id, PTTDB_CONTENT_TYPE_COMMENT_REPLY, p_comment->comment_reply_id, 0, 0, "\n", 1);
    }

    PttUIBuffer *p_tmp = NULL;
    // buffer-info
    if(!error_code) {
        p_tmp = malloc(sizeof(PttUIBuffer));
        memcpy(p_tmp->the_id, p_comment->comment_reply_id, UUIDLEN);
        p_tmp->content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
        p_tmp->block_offset = 0;
        p_tmp->line_offset = 0;
        p_tmp->comment_offset = comment_offset;

        p_tmp->load_line_offset = 0;
        p_tmp->load_line_pre_offset = INVALID_LINE_OFFSET_PRE_END;
        p_tmp->load_line_next_offset = INVALID_LINE_OFFSET_NEXT_END;

        p_tmp->file_offset = 0;
        p_tmp->file_line_offset = 0;
        p_tmp->file_line_pre_offset = INVALID_LINE_OFFSET_PRE_END;
        p_tmp->file_line_next_offset = INVALID_LINE_OFFSET_NEXT_END;

        p_tmp->storage_type = PTTDB_STORAGE_TYPE_FILE;

        p_tmp->is_modified = false;
        p_tmp->is_new = false;
        p_tmp->is_to_delete = false;
        p_tmp->len_no_nl = 0;
        p_tmp->buf = malloc(MAX_TEXTLINE_SIZE + 1);
        p_tmp->buf[0] = 0;

        p_tmp->next = VEDIT3_EDITOR_STATUS.current_buffer->next;
        p_tmp->pre = VEDIT3_EDITOR_STATUS.current_buffer;

        VEDIT3_EDITOR_STATUS.current_buffer->next = p_tmp;
        if(p_tmp->next) p_tmp->next->pre = p_tmp;

        if(PTTUI_BUFFER_INFO.tail == VEDIT3_EDITOR_STATUS.current_buffer) {
            PTTUI_BUFFER_INFO.tail = p_tmp;
        }

        PTTUI_BUFFER_INFO.n_buffer++;
    }

    // unlock
    Err error_code_lock = vedit3_repl_wrunlock_file_info_buffer_info(is_lock_file_info, is_lock_wr_buffer_info, is_lock_buffer_info);
    if(!error_code && error_code_lock) error_code = error_code_lock;

    if(!error_code) {
        error_code = vedit3_action_move_down();
    }

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return error_code;
}

Err
vedit3_action_comment_delete_comment()
{
    return S_OK;
}

/**
 * @brief [brief description]
 * @details refer: Ctrl('V') in vedit2 in edit.c
 */
Err
vedit3_action_toggle_ansi()
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
vedit3_action_t_users()
{
    t_users();
    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return S_OK;
}

Err
vedit3_action_insert_char(int ch)
{
    Err error_code = S_OK;
    char *pstr = NULL;
    if(VEDIT3_EDITOR_STATUS.is_phone) {
        error_code = pttui_phone_char(ch, VEDIT3_EDITOR_STATUS.phone_mode, &pstr);
        if(error_code) return error_code;
    }

    bool is_lock_file_info = false;
    bool is_lock_wr_buffer_info = false;
    bool is_lock_buffer_info = false;

    PttUIBuffer *tmp_tail = NULL;
    error_code = vedit3_repl_wrlock_file_info_buffer_info(&is_lock_file_info, &is_lock_wr_buffer_info, &is_lock_buffer_info);

    if(!error_code) {
        error_code = pstr ? _vedit3_action_insert_dchar_core(pstr) : _vedit3_action_insert_char_core(ch);
    }

    tmp_tail = PTTUI_BUFFER_INFO.tail;

    Err error_code_lock = vedit3_repl_wrunlock_file_info_buffer_info(is_lock_file_info, is_lock_wr_buffer_info, is_lock_buffer_info);
    if(!error_code && error_code_lock) error_code = error_code_lock;

    if(!error_code && VEDIT3_EDITOR_STATUS.current_col > VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl && pttui_buffer_next_ne(VEDIT3_EDITOR_STATUS.current_buffer, tmp_tail)) {
        VEDIT3_EDITOR_STATUS.current_col = VEDIT3_EDITOR_STATUS.current_col - VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl;
        error_code = vedit3_action_move_down();
    }

    return error_code;
}

Err
vedit3_action_insert_new_line()
{
    PttUIBuffer *new_buffer = NULL;

    bool is_lock_file_info = false;
    bool is_lock_wr_buffer_info = false;
    bool is_lock_buffer_info = false;

    Err error_code = vedit3_repl_wrlock_file_info_buffer_info(&is_lock_file_info, &is_lock_wr_buffer_info, &is_lock_buffer_info);

    if(!error_code) {
        error_code = _vedit3_action_buffer_split_core(VEDIT3_EDITOR_STATUS.current_buffer, VEDIT3_EDITOR_STATUS.current_col, 0, &new_buffer);
    }

    Err error_code_lock = vedit3_repl_wrunlock_file_info_buffer_info(is_lock_file_info, is_lock_wr_buffer_info, is_lock_buffer_info);
    if(!error_code && error_code_lock) error_code = error_code_lock;

    if(!error_code) {
        error_code = vedit3_action_move_down();
    }

    if(!error_code) {
        error_code = vedit3_action_move_begin_line();
    }

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return error_code;
}

Err
vedit3_action_move_right()
{
    Err error_code = S_OK;
    int ansi_current_col = 0;
    int mbcs_current_col = 0;
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

        return S_OK;
    }

    // need to move to next line

    // move to next line
    error_code = vedit3_action_move_down();

    VEDIT3_EDITOR_STATUS.current_col = 0;

    return error_code;
}

Err
vedit3_action_move_left()
{
    Err error_code = S_OK;
    int ansi_current_col = 0;
    int mbcs_current_col = 0;
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

        return S_OK;
    }

    // begin of the file. (pos: 0, 0) no need to do anything (move_up take cake of line only, not the col)
    if(VEDIT3_EDITOR_STATUS.current_buffer_line == 0 && VEDIT3_EDITOR_STATUS.current_col == 0) return S_OK;

    // move to previous line
    error_code = vedit3_action_move_up();

    VEDIT3_EDITOR_STATUS.current_col = VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl;

    return error_code;
}

Err
vedit3_action_move_up()
{
    Err error_code = S_OK;
    // is begin of file
    bool is_begin = false;
    error_code = pttui_buffer_is_begin_of_file(VEDIT3_EDITOR_STATUS.current_buffer, &PTTUI_FILE_INFO, &is_begin);
    if (error_code) return error_code;
    if (is_begin) return S_OK;

    // check begin-of-window
    error_code = _vedit3_action_move_up_ensure_top_of_window();
    if (error_code) return error_code;

    // move next
    error_code = vedit3_repl_lock_buffer_info();
    if (error_code) return error_code;

    PttUIBuffer *pre_buffer = pttui_buffer_pre_ne(VEDIT3_EDITOR_STATUS.current_buffer, PTTUI_BUFFER_INFO.head);
    if(!pre_buffer) error_code = S_ERR;
    if(!error_code) {
        VEDIT3_EDITOR_STATUS.current_buffer = pre_buffer;
    }

    Err error_code_lock = vedit3_repl_unlock_buffer_info();
    if (!error_code && error_code_lock) error_code = S_ERR_EDIT_LOCK;

    if (VEDIT3_EDITOR_STATUS.current_line > 0) {
        VEDIT3_EDITOR_STATUS.current_line--;
    }
    else {
        VEDIT3_EDITOR_STATUS.is_scroll_down = true;
    }
    VEDIT3_EDITOR_STATUS.current_buffer_line--;

    Err error_code2 = _vedit3_action_ensure_current_col(VEDIT3_EDITOR_STATUS.current_col);
    if(!error_code && error_code2) error_code = error_code2;

    return error_code;
}

Err
vedit3_action_move_down()
{
    Err error_code = S_OK;
    // is end of file
    bool is_eof = false;
    error_code = pttui_buffer_is_eof(VEDIT3_EDITOR_STATUS.current_buffer, &PTTUI_FILE_INFO, &is_eof);
    fprintf(stderr, "vedit3_action.vedit3_action_move_down: after is_eof: is_eof: %d e: %d\n", is_eof, error_code);
    if (error_code) return error_code;
    if (is_eof) return S_OK;

    // check end-of-window
    error_code = _vedit3_action_move_down_ensure_end_of_window();
    fprintf(stderr, "vedit3_action.vedit3_action_move_down: after ensure end of window: e: %d\n", error_code);
    if (error_code) return error_code;

    // move next
    error_code = vedit3_repl_lock_buffer_info();
    if (error_code) return error_code;

    fprintf(stderr, "vedit3_action.vedit3_action_move_down: to next-ne: e: %d\n", error_code);

    PttUIBuffer *next_buffer = pttui_buffer_next_ne(VEDIT3_EDITOR_STATUS.current_buffer, PTTUI_BUFFER_INFO.tail);
    if(!next_buffer) error_code = S_ERR;
    fprintf(stderr, "vedit3_action.vedit3_action_move_down: after next-ne: e: %d\n", error_code);
    if(!error_code) {
        VEDIT3_EDITOR_STATUS.current_buffer = next_buffer;
    }

    Err error_code_lock = vedit3_repl_unlock_buffer_info();
    if (error_code_lock) error_code = S_ERR_EDIT_LOCK;

    if (VEDIT3_EDITOR_STATUS.current_line < b_lines - 1) {
        VEDIT3_EDITOR_STATUS.current_line++;
    }
    else {
        VEDIT3_EDITOR_STATUS.is_scroll_up = true;
    }
    VEDIT3_EDITOR_STATUS.current_buffer_line++;

    Err error_code2 = _vedit3_action_ensure_current_col(VEDIT3_EDITOR_STATUS.current_col);
    if(!error_code && error_code2) error_code = error_code2;

    fprintf(stderr, "vedit3_action.vedit3_action_move_down: after ensure-current-col: e: %d\n", error_code);

    return error_code;
}

Err
vedit3_action_move_end_line()
{
    VEDIT3_EDITOR_STATUS.current_col = VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl;

    return S_OK;
}

Err
vedit3_action_move_begin_line()
{
    VEDIT3_EDITOR_STATUS.current_col = 0;

    return S_OK;
}

Err
vedit3_action_move_pgup()
{
    Err error_code = S_OK;
    int current_col = VEDIT3_EDITOR_STATUS.current_col;

    PttUIState expected_state = {};
    int n_pre_line = 0;
    error_code = _vedit3_action_move_pgup_get_expected_top_line_buffer(&VEDIT3_EDITOR_STATUS, &PTTUI_FILE_INFO, &PTTUI_STATE, &expected_state, &n_pre_line);
    if(error_code) return error_code;

    error_code = pttui_set_expected_state(expected_state.main_id, expected_state.top_line_content_type, expected_state.top_line_id, expected_state.top_line_block_offset, expected_state.top_line_line_offset, expected_state.top_line_comment_offset, expected_state.n_window_line);
    if (error_code) return error_code;

    error_code = vedit3_wait_buffer_state_sync(DEFAULT_ITER_VEDIT3_WAIT_BUFFER_STATE_SYNC);
    if(error_code) return error_code;

    error_code = vedit3_repl_lock_buffer_info();
    if(error_code) return error_code;

    PttUIBuffer *p_buffer = NULL;
    int i = 0;

    if(n_pre_line) {
        p_buffer = PTTUI_BUFFER_TOP_LINE;
        for(i = 0; i < VEDIT3_EDITOR_STATUS.current_line && p_buffer != PTTUI_BUFFER_INFO.tail; i++, p_buffer = pttui_buffer_next_ne(p_buffer, PTTUI_BUFFER_INFO.tail));
    }
    else {
        p_buffer = PTTUI_BUFFER_TOP_LINE;
        n_pre_line = VEDIT3_EDITOR_STATUS.current_line;
    }

    VEDIT3_EDITOR_STATUS.current_buffer = p_buffer;
    VEDIT3_EDITOR_STATUS.current_line = i;
    VEDIT3_EDITOR_STATUS.current_buffer_line -= n_pre_line;

    Err error_code2 = vedit3_repl_unlock_buffer_info();
    if(!error_code && error_code2) error_code = error_code2;

    error_code = _vedit3_action_ensure_current_col(current_col);

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return error_code;
}

Err
_vedit3_action_move_pgup_get_expected_top_line_buffer(VEdit3EditorStatus *editor_status, FileInfo *file_info, PttUIState *current_state, PttUIState *expected_state, int *n_pre_line)
{
    Err error_code = S_OK;

    int n_window_line = b_lines;

    memcpy(expected_state, current_state, sizeof(PttUIState));

    int current_buffer_top_line = editor_status->current_buffer_line - editor_status->current_line;
    int tmp_n_pre_line = current_buffer_top_line < b_lines ? current_buffer_top_line : b_lines;

    bool is_lock_file_info = false;
    bool is_lock_buffer_info = false;
    Err error_code_lock = vedit3_repl_rdlock_file_info_buffer_info(&is_lock_file_info, &is_lock_buffer_info);
    if(!error_code && error_code_lock) error_code = error_code_lock;

    PttUIBuffer *current_buffer = PTTUI_BUFFER_TOP_LINE;

    bool is_first_line = false;
    PttUIBuffer tmp_buffer = {};
    PttUIBuffer tmp_buffer2 = {};
    memcpy(&tmp_buffer, current_buffer, sizeof(PttUIBuffer));
    int i = 0;
    if(!error_code) {
        for(i = 0; i < tmp_n_pre_line; i++) {
            error_code = file_info_is_first_line(file_info, tmp_buffer.content_type, tmp_buffer.comment_offset, tmp_buffer.block_offset, tmp_buffer.line_offset, &is_first_line);
            fprintf(stderr, "vedit3_action._vedit3_action_move_pgup_get_expected_top_line_buffer: (%d/%d) after is_first_line: (content_type: %d comment: %d block: %d line: %d) is_first_line: %d\n", i, tmp_n_pre_line, tmp_buffer.content_type, tmp_buffer.comment_offset, tmp_buffer.block_offset, tmp_buffer.line_offset, is_first_line);
            if(error_code) break;
            if(is_first_line) break;
            error_code = file_info_get_pre_line(file_info, tmp_buffer.the_id,
                tmp_buffer.content_type, tmp_buffer.block_offset, tmp_buffer.line_offset, tmp_buffer.comment_offset,
                tmp_buffer2.the_id,
                &tmp_buffer2.content_type, &tmp_buffer2.block_offset, &tmp_buffer2.line_offset, &tmp_buffer2.comment_offset,
                &tmp_buffer2.storage_type);
            fprintf(stderr, "vedit3_action._vedit3_action_move_pgup_get_expected_top_line_buffer: (%d/%d) after get_pre_line: (content_type: %d comment: %d block: %d line: %d) new: (content_type: %d comment: %d block: %d line: %d) e: %d\n", i, tmp_n_pre_line, tmp_buffer.content_type, tmp_buffer.comment_offset, tmp_buffer.block_offset, tmp_buffer.line_offset, tmp_buffer2.content_type, tmp_buffer2.comment_offset, tmp_buffer2.block_offset, tmp_buffer2.line_offset, error_code);
            if(error_code) break;

            memcpy(&tmp_buffer, &tmp_buffer2, sizeof(PttUIBuffer));
        }
    }

    if(!error_code) {
        memcpy(expected_state->main_id, current_state->main_id, UUIDLEN);
        expected_state->top_line_content_type = tmp_buffer.content_type;
        expected_state->top_line_block_offset = tmp_buffer.block_offset;
        expected_state->top_line_line_offset = tmp_buffer.line_offset;
        expected_state->top_line_comment_offset = tmp_buffer.comment_offset;
        expected_state->n_window_line = n_window_line;
        *n_pre_line = i;
    }

    error_code_lock = vedit3_repl_unlock_file_info_buffer_info(is_lock_file_info, is_lock_buffer_info);
    if(!error_code && error_code_lock) error_code = error_code_lock;

    return S_OK;
}

Err
vedit3_action_move_pgdn()
{
    Err error_code = S_OK;
    int current_col = VEDIT3_EDITOR_STATUS.current_col;

    PttUIState expected_state = {};
    int n_next_line = 0;
    error_code = _vedit3_action_move_pgdn_get_expected_top_line_buffer(&VEDIT3_EDITOR_STATUS, &PTTUI_FILE_INFO, &PTTUI_STATE, &expected_state, &n_next_line);
    fprintf(stderr, "vedit3_action.vedit3_action_move_pgdn: get expected top line buffer: e: %d n_next_line: %d\n", error_code, n_next_line);
    if(error_code) return error_code;

    error_code = pttui_set_expected_state(expected_state.main_id, expected_state.top_line_content_type, expected_state.top_line_id, expected_state.top_line_block_offset, expected_state.top_line_line_offset, expected_state.top_line_comment_offset, expected_state.n_window_line);
    //fprintf(stderr, "vedit3_action.vedit3_action_move_pgdn: after set expected_state: e: %d\n", error_code);
    if (error_code) return error_code;

    error_code = vedit3_wait_buffer_state_sync(DEFAULT_ITER_VEDIT3_WAIT_BUFFER_STATE_SYNC);
    //fprintf(stderr, "vedit3_action.vedit3_action_move_pgdn: after wait buffer_state_sync: e: %d\n", error_code);
    if(error_code) return error_code;

    error_code = vedit3_repl_lock_buffer_info();
    if(error_code) return error_code;

    PttUIBuffer *p_buffer = NULL;
    int i = 0;
    if(n_next_line) {
        n_next_line -= VEDIT3_EDITOR_STATUS.current_line;
        p_buffer = PTTUI_BUFFER_TOP_LINE;
        for(i = 0;
            i < VEDIT3_EDITOR_STATUS.current_line && p_buffer != PTTUI_BUFFER_INFO.tail;
            i++, p_buffer = pttui_buffer_next_ne(p_buffer, PTTUI_BUFFER_INFO.tail), n_next_line++);

    }
    else {
        VEDIT3_EDITOR_STATUS.current_buffer_line -= VEDIT3_EDITOR_STATUS.current_line;
        VEDIT3_EDITOR_STATUS.current_line = 0;
        p_buffer = PTTUI_BUFFER_TOP_LINE;
        for(i = 0;
            p_buffer != PTTUI_BUFFER_INFO.tail;
            i++, p_buffer = pttui_buffer_next_ne(p_buffer, PTTUI_BUFFER_INFO.tail), n_next_line++);
    }

    VEDIT3_EDITOR_STATUS.current_buffer = p_buffer;
    VEDIT3_EDITOR_STATUS.current_line = i;
    VEDIT3_EDITOR_STATUS.current_buffer_line += n_next_line;

    log_file_info(&PTTUI_FILE_INFO, "vedit3_action.vedit3_action_move_pgdn: file_info");
    log_pttui_buffer_info(&PTTUI_BUFFER_INFO, "vedit3_action.vedit3_action_move_pgdn: buffer_info");
    fprintf(stderr, "vedit3_action.current_buffer: (content-type: %d comment: %d block: %d line: %d load-line: (%d pre: %d next: %d) file: %d file-line: (%d pre: %d next: %d)) current_line: %d current_buffer_line: %d\n",
        p_buffer->content_type, p_buffer->comment_offset, p_buffer->block_offset, p_buffer->line_offset,
        p_buffer->load_line_offset, p_buffer->load_line_pre_offset, p_buffer->load_line_next_offset,
        p_buffer->file_offset,
        p_buffer->file_line_offset, p_buffer->file_line_pre_offset, p_buffer->file_line_next_offset, VEDIT3_EDITOR_STATUS.current_line, VEDIT3_EDITOR_STATUS.current_buffer_line
        );

    Err error_code2 = vedit3_repl_unlock_buffer_info();
    if(!error_code && error_code2) error_code = error_code2;

    error_code = _vedit3_action_ensure_current_col(current_col);

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return error_code;
}

Err
_vedit3_action_move_pgdn_get_expected_top_line_buffer(VEdit3EditorStatus *editor_status, FileInfo *file_info, PttUIState *current_state, PttUIState *expected_state, int *n_next_line)
{
    Err error_code = S_OK;

    int n_window_line = b_lines;

    memcpy(expected_state, current_state, sizeof(PttUIState));

    int current_buffer_top_line = editor_status->current_buffer_line - editor_status->current_line;

    int tmp_n_next_line = file_info->n_total_line - current_buffer_top_line < b_lines ? 0 : b_lines;

    bool is_lock_file_info = false;
    bool is_lock_buffer_info = false;
    Err error_code_lock = vedit3_repl_rdlock_file_info_buffer_info(&is_lock_file_info, &is_lock_buffer_info);
    if(!error_code && error_code_lock) error_code = error_code_lock;

    PttUIBuffer *current_buffer = PTTUI_BUFFER_TOP_LINE;

    bool is_last_line = false;
    PttUIBuffer tmp_buffer = {};
    PttUIBuffer tmp_buffer2 = {};
    memcpy(&tmp_buffer, current_buffer, sizeof(PttUIBuffer));
    int i = 0;
    if(!error_code) {
        for(i = 0; i < tmp_n_next_line; i++) {
            error_code = file_info_is_last_line(file_info, tmp_buffer.content_type, tmp_buffer.comment_offset, tmp_buffer.block_offset, tmp_buffer.line_offset, &is_last_line);
            fprintf(stderr, "vedit3_action._vedit3_action_move_pgdn_get_expected_top_line_buffer: (%d/%d) after is_last_line: (content_type: %d comment: %d block: %d line: %d) is_last_line: %d\n", i, tmp_n_next_line, tmp_buffer.content_type, tmp_buffer.comment_offset, tmp_buffer.block_offset, tmp_buffer.line_offset, is_last_line);
            if(error_code) break;
            if(is_last_line) break;

            error_code = file_info_get_next_line(file_info, tmp_buffer.the_id, tmp_buffer.content_type, tmp_buffer.block_offset, tmp_buffer.line_offset, tmp_buffer.comment_offset, tmp_buffer2.the_id, &tmp_buffer2.content_type, &tmp_buffer2.block_offset, &tmp_buffer2.line_offset, &tmp_buffer2.comment_offset, &tmp_buffer2.storage_type);
            fprintf(stderr, "vedit3_action._vedit3_action_move_pgdn_get_expected_top_line_buffer: (%d/%d) after get_next_line: (content_type: %d comment: %d block: %d line: %d) new: (content_type: %d comment: %d block: %d line: %d) e: %d\n", i, tmp_n_next_line, tmp_buffer.content_type, tmp_buffer.comment_offset, tmp_buffer.block_offset, tmp_buffer.line_offset, tmp_buffer2.content_type, tmp_buffer2.comment_offset, tmp_buffer2.block_offset, tmp_buffer2.line_offset, error_code);

            if(error_code) break;

            memcpy(&tmp_buffer, &tmp_buffer2, sizeof(PttUIBuffer));
        }
    }

    if(!error_code) {
        memcpy(expected_state->main_id, current_state->main_id, UUIDLEN);
        expected_state->top_line_content_type = tmp_buffer.content_type;
        expected_state->top_line_block_offset = tmp_buffer.block_offset;
        expected_state->top_line_line_offset = tmp_buffer.line_offset;
        expected_state->top_line_comment_offset = tmp_buffer.comment_offset;
        expected_state->n_window_line = n_window_line;
        *n_next_line = i;
    }

    error_code_lock = vedit3_repl_unlock_file_info_buffer_info(is_lock_file_info, is_lock_buffer_info);
    if(!error_code && error_code_lock) error_code = error_code_lock;

    return error_code;
}

Err
vedit3_action_move_start_file()
{
    return S_OK;
}

Err
vedit3_action_move_tail_file()
{
    return S_OK;
}

Err
vedit3_action_redraw()
{
    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return S_OK;
}

Err
vedit3_action_show_help()
{
    more("etc/ve.hlp", true);

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return S_OK;
}

/**
 * @brief
 * @details ref: case KEY_BS in edit.c
 *          1. is-ansi: is-ansi == false
 *          2. is current-line == 0 and current-col == 0: no change
 *          3. current-col == 0: move to previous-line, try to concat the next line.
 *          4. move-left. do delete-char.
 */
Err
vedit3_action_backspace()
{
    Err error_code = S_OK;
    // XXX remove block cancel for now
    //block_cancel();

    // is-ansi: is-ansi = false
    if(VEDIT3_EDITOR_STATUS.is_ansi) {
        VEDIT3_EDITOR_STATUS.is_ansi = false;
        VEDIT3_EDITOR_STATUS.is_redraw_everything = true;
        return S_OK;
    }

    // current-buffer-line == 0 and current-col == 0: no change
    if(VEDIT3_EDITOR_STATUS.current_buffer_line == 0 && VEDIT3_EDITOR_STATUS.current_col == 0) return S_OK;

    // current-col == 0: move to previous-line, try to concat the next line.s
    bool is_lock_file_info = false;
    bool is_lock_wr_buffer_info = false;
    bool is_lock_buffer_info = false;
    Err error_code_lock = S_OK;

    if(VEDIT3_EDITOR_STATUS.current_col == 0) {
        error_code = vedit3_action_move_left();
        if(error_code) return error_code;

        error_code = vedit3_repl_wrlock_file_info_buffer_info(&is_lock_file_info, &is_lock_wr_buffer_info, &is_lock_buffer_info);

        if(!error_code) {
            error_code = _vedit3_action_concat_next_line();
        }

        error_code_lock = vedit3_repl_wrunlock_file_info_buffer_info(is_lock_file_info, is_lock_wr_buffer_info, is_lock_buffer_info);
        if(!error_code && error_code_lock) error_code = error_code_lock;

        return error_code;
    }

    error_code = vedit3_action_move_left();
    if(error_code) return error_code;

    error_code = vedit3_action_delete_char();

    return error_code;
}

Err
vedit3_action_delete_char()
{
    Err error_code = S_OK;
    Err error_code_lock = S_OK;

    bool is_lock_file_info = false;
    bool is_lock_wr_buffer_info = false;
    bool is_lock_buffer_info = false;

    error_code = vedit3_repl_wrlock_file_info_buffer_info(&is_lock_file_info, &is_lock_wr_buffer_info, &is_lock_buffer_info);

    if(!error_code && VEDIT3_EDITOR_STATUS.current_col == VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl) { // last char
        error_code = _vedit3_action_concat_next_line();

        error_code_lock = vedit3_repl_wrunlock_file_info_buffer_info(is_lock_file_info, is_lock_wr_buffer_info, is_lock_buffer_info);
        if(!error_code && error_code_lock) error_code = error_code_lock;

        return error_code;
    }

    // not last char

    int w = VEDIT3_EDITOR_STATUS.is_mbcs ? pttui_mchar_len_ne((unsigned char*)(VEDIT3_EDITOR_STATUS.current_buffer->buf + VEDIT3_EDITOR_STATUS.current_col)) : 1;

    if(!error_code) {
        for(int i = 0; i < w; i++) {
            error_code = _vedit3_action_delete_char_core();
            if(error_code) break;
        }
    }

    int ansi_current_col = 0;
    if(!error_code && VEDIT3_EDITOR_STATUS.is_ansi) {
        error_code = pttui_n2ansi(VEDIT3_EDITOR_STATUS.current_col, VEDIT3_EDITOR_STATUS.current_buffer->buf, &ansi_current_col);
    }

    if(!error_code && VEDIT3_EDITOR_STATUS.is_ansi) {
        error_code = pttui_ansi2n(ansi_current_col, VEDIT3_EDITOR_STATUS.current_buffer->buf, &VEDIT3_EDITOR_STATUS.current_col);
    }

    VEDIT3_EDITOR_STATUS.current_buffer->is_modified = true;

    error_code_lock = vedit3_repl_wrunlock_file_info_buffer_info(is_lock_file_info, is_lock_wr_buffer_info, is_lock_buffer_info);
    if(!error_code && error_code_lock) error_code = error_code_lock;

    return S_OK;
}

Err
vedit3_action_delete_line()
{
    VEDIT3_EDITOR_STATUS.current_col = 0;

    return vedit3_action_delete_end_of_line();
}

Err
vedit3_action_delete_end_of_line()
{
    VEDIT3_EDITOR_STATUS.current_buffer->is_modified = true;

    if(VEDIT3_EDITOR_STATUS.current_col != VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl) {
        VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl = VEDIT3_EDITOR_STATUS.current_col;
        VEDIT3_EDITOR_STATUS.current_buffer->buf[VEDIT3_EDITOR_STATUS.current_col] = 0;
        return S_OK;
    }

    bool is_lock_file_info = false;
    bool is_lock_wr_buffer_info = false;
    bool is_lock_buffer_info = false;
    Err error_code_lock = S_OK;
    Err error_code = vedit3_repl_wrlock_file_info_buffer_info(&is_lock_file_info, &is_lock_wr_buffer_info, &is_lock_buffer_info);

    if(!error_code) {
        error_code = _vedit3_action_concat_next_line();
    }

    error_code_lock = vedit3_repl_wrunlock_file_info_buffer_info(is_lock_file_info, is_lock_wr_buffer_info, is_lock_buffer_info);
    if(!error_code && error_code_lock) error_code = error_code_lock;

    return error_code;
}


/********************
 * action-util
 ********************/

Err
_vedit3_action_insert_dchar_core(const char *dchar)
{
    if (!VEDIT3_EDITOR_STATUS.is_own_wrlock_buffer_info) return S_ERR_EDIT_LOCK;

    Err error_code = _vedit3_action_insert_char_core(*dchar);
    Err error_code2 = _vedit3_action_insert_char_core(*(dchar+1));

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
_vedit3_action_insert_char_core(int ch)
{
    Err error_code = S_OK;

    if (!VEDIT3_EDITOR_STATUS.is_own_wrlock_buffer_info) return S_ERR_EDIT_LOCK;

    PttUIBuffer *current_buffer = VEDIT3_EDITOR_STATUS.current_buffer;

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

    if (!VEDIT3_EDITOR_STATUS.is_own_wrlock_buffer_info) return S_ERR_EDIT_LOCK;

    PttUIBuffer *current_buffer = VEDIT3_EDITOR_STATUS.current_buffer;

    if (current_buffer->len_no_nl < WRAPMARGIN) {
        return S_OK;
    }

    bool is_wordwrap = true;
    char *s = current_buffer->buf + current_buffer->len_no_nl - 1;
    while (s != current_buffer->buf && *s == ' ') s--;
    while (s != current_buffer->buf && *s != ' ') s--;
    if (s == current_buffer->buf) { // if only 1 word
        is_wordwrap = false;
        s = current_buffer->buf + (current_buffer->len_no_nl - 2);
    }

    PttUIBuffer *new_buffer = NULL;

    error_code = _vedit3_action_buffer_split_core(current_buffer, s - current_buffer->buf + 1, 0, &new_buffer);

    int new_buffer_len_no_nl = new_buffer ? new_buffer->len_no_nl : 0;
    if (!error_code && is_wordwrap && new_buffer && new_buffer_len_no_nl >= 1) {
        if (new_buffer->buf[new_buffer_len_no_nl - 1] != ' ') {
            new_buffer->buf[new_buffer_len_no_nl] = ' ';
            new_buffer->len_no_nl++;
            new_buffer->buf[new_buffer_len_no_nl] = '\0';
        }
    }

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return error_code;
}

Err
_vedit3_action_ensure_current_col(int current_col) {
    VEDIT3_EDITOR_STATUS.current_col = (current_col < VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl || VEDIT3_EDITOR_STATUS.current_buffer->content_type == PTTDB_CONTENT_TYPE_COMMENT) ? current_col : VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl;

    return S_OK;
}

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param current_buffer [description]
 * @param pos [description]
 * @param indent [description]
 * @param new_buffer [description]
 */
Err
_vedit3_action_buffer_split_core(PttUIBuffer *current_buffer, int pos, int indent, PttUIBuffer **new_buffer)
{
    Err error_code = S_OK;

    if (!VEDIT3_EDITOR_STATUS.is_own_wrlock_buffer_info) return S_ERR_EDIT_LOCK;

    // XXX should not happen.
    if(pos > current_buffer->len_no_nl) return S_OK;

    *new_buffer = malloc(sizeof(PttUIBuffer));
    PttUIBuffer *p_new_buffer = *new_buffer;
    bzero(p_new_buffer, sizeof(PttUIBuffer));

    memcpy(p_new_buffer->the_id, current_buffer->the_id, UUIDLEN);
    p_new_buffer->content_type = current_buffer->content_type;
    p_new_buffer->block_offset = current_buffer->block_offset;
    p_new_buffer->line_offset = current_buffer->line_offset + 1;
    p_new_buffer->comment_offset = current_buffer->comment_offset;
    p_new_buffer->load_line_offset = INVALID_LINE_OFFSET_NEW;
    p_new_buffer->load_line_pre_offset = current_buffer->load_line_offset;
    p_new_buffer->load_line_next_offset = current_buffer->load_line_next_offset;
    current_buffer->load_line_next_offset = INVALID_LINE_OFFSET_NEW;

    p_new_buffer->storage_type = PTTDB_STORAGE_TYPE_OTHER;

    p_new_buffer->is_modified = true;
    p_new_buffer->is_new = true;
    current_buffer->is_modified = true;

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

    current_buffer->len_no_nl = pos;
    current_buffer->buf[current_buffer->len_no_nl] = 0;

    error_code = pttui_buffer_insert_buffer(current_buffer, p_new_buffer);

    // buffer after new_buffer
    for(PttUIBuffer *p_buffer2 = pttui_buffer_next_ne(p_new_buffer, PTTUI_BUFFER_INFO.tail); p_buffer2 && p_buffer2->content_type == p_new_buffer->content_type && p_buffer2->block_offset == p_new_buffer->block_offset && p_buffer2->comment_offset == p_new_buffer->comment_offset; p_buffer2->line_offset++, p_buffer2 = pttui_buffer_next_ne(p_buffer2, PTTUI_BUFFER_INFO.tail));

    // file-info
    switch(current_buffer->content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        error_code = file_info_increase_main_content_line(&PTTUI_FILE_INFO, current_buffer->block_offset, current_buffer->file_offset);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        error_code = file_info_increase_comment_reply_line(&PTTUI_FILE_INFO, current_buffer->comment_offset, current_buffer->block_offset, current_buffer->file_offset);
        break;
    default:
        break;
    }

    // buffer-info
    // BASIC_OP. directly use ->next
    if(PTTUI_BUFFER_INFO.tail == current_buffer) {
        PTTUI_BUFFER_INFO.tail = current_buffer->next;
    }
    PTTUI_BUFFER_INFO.n_buffer++;
    PTTUI_BUFFER_INFO.n_new++;

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
    memcpy(main_id, PTTUI_STATE.main_id, UUIDLEN);
    int n_window_line = b_lines;

    UUID new_id = {};
    enum PttDBContentType new_content_type = PTTDB_CONTENT_TYPE_MAIN;
    int new_block_offset = 0;
    int new_line_offset = 0;
    int new_comment_offset = 0;
    enum PttDBStorageType _dummy = PTTDB_STORAGE_TYPE_MONGO;

    Err error_code = file_info_get_pre_line(&PTTUI_FILE_INFO, PTTUI_STATE.top_line_id, PTTUI_STATE.top_line_content_type, PTTUI_STATE.top_line_block_offset, PTTUI_STATE.top_line_line_offset, PTTUI_STATE.top_line_comment_offset, new_id, &new_content_type, &new_block_offset, &new_line_offset, &new_comment_offset, &_dummy);
    if (error_code) return error_code;

    error_code = pttui_set_expected_state(main_id, new_content_type, new_id, new_block_offset, new_line_offset, new_comment_offset, n_window_line);
    if (error_code) return error_code;

    error_code = vedit3_wait_buffer_state_sync(DEFAULT_ITER_VEDIT3_WAIT_BUFFER_STATE_SYNC);

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
    memcpy(main_id, PTTUI_STATE.main_id, UUIDLEN);
    int n_window_line = b_lines;

    UUID new_id = {};
    enum PttDBContentType new_content_type = PTTDB_CONTENT_TYPE_MAIN;
    int new_block_offset = 0;
    int new_line_offset = 0;
    int new_comment_offset = 0;
    enum PttDBStorageType _dummy = PTTDB_STORAGE_TYPE_MONGO;

    Err error_code = file_info_get_next_line(&PTTUI_FILE_INFO, PTTUI_STATE.top_line_id, PTTUI_STATE.top_line_content_type, PTTUI_STATE.top_line_block_offset, PTTUI_STATE.top_line_line_offset, PTTUI_STATE.top_line_comment_offset, new_id, &new_content_type, &new_block_offset, &new_line_offset, &new_comment_offset, &_dummy);
    fprintf(stderr, "vedit3_action._vedit3_action_move_down_ensure_end_of_window: after file_info_get_next_line: orig-top-line: (content-type: %d comment: %d block: %d line: %d) new: (content-type: %d comment: %d block: %d line: %d)\n",
        PTTUI_STATE.top_line_content_type, PTTUI_STATE.top_line_comment_offset, PTTUI_STATE.top_line_block_offset, PTTUI_STATE.top_line_line_offset,
        new_content_type, new_block_offset, new_line_offset, new_comment_offset);
    if (error_code) return error_code;

    error_code = pttui_set_expected_state(main_id, new_content_type, new_id, new_block_offset, new_line_offset, new_comment_offset, n_window_line);
    if (error_code) return error_code;

    error_code = vedit3_wait_buffer_state_sync(DEFAULT_ITER_VEDIT3_WAIT_BUFFER_STATE_SYNC);
    if (error_code) return error_code;

    return error_code;
}

Err
_vedit3_action_delete_char_core()
{
    if (!VEDIT3_EDITOR_STATUS.is_own_wrlock_buffer_info) return S_ERR_EDIT_LOCK;

    if(!VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl) return S_OK;

    Err error_code = pttui_raw_shift_left(VEDIT3_EDITOR_STATUS.current_buffer->buf + VEDIT3_EDITOR_STATUS.current_col, VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl - VEDIT3_EDITOR_STATUS.current_col + 1);
    if(error_code) return error_code;

    VEDIT3_EDITOR_STATUS.current_buffer->len_no_nl--;

    VEDIT3_EDITOR_STATUS.current_buffer->is_modified = true;

    return S_OK;
}

/**
 * @brief [brief description]
 * @details assume in the end of line (current_col == current_buffer->len_no_nl)
 *          1. if no next: return
 *          2. if no next-non-space-char: delete next line
 *          3. if no overflow:
 *             3.1 concat
 *             3.2 delete next line
 *          4. 4.1. get first_word
 *             4.2. 如果 len + first_word >= WRAPMARGIN: no change.
 *             4.3. else: concat first-word.
 *                        if no second-word: delete next-line
 *                        else: shift-line.
 */
Err
_vedit3_action_concat_next_line()
{
    Err error_code = S_OK;

    if (!VEDIT3_EDITOR_STATUS.is_own_wrlock_buffer_info) return S_ERR_EDIT_LOCK;

    PttUIBuffer *current_buffer = VEDIT3_EDITOR_STATUS.current_buffer;

    int n_line = 0;
    error_code = file_info_get_n_line(&PTTUI_FILE_INFO, current_buffer->content_type, current_buffer->comment_offset, current_buffer->block_offset, &n_line);
    if(error_code) return error_code;

    fprintf(stderr, "vedit3_action._vedit3_action_concat_next_line: buffer: (content_type: %d comment: %d block: %d line: %d file: %d load_line_next_offset: %d) n_line: %d\n", current_buffer->content_type, current_buffer->comment_offset, current_buffer->block_offset, current_buffer->line_offset, current_buffer->file_offset, current_buffer->load_line_next_offset, n_line);

    if(file_info_is_last_block_ne(&PTTUI_FILE_INFO, current_buffer->content_type, current_buffer->comment_offset, current_buffer->block_offset) && current_buffer->line_offset == n_line - 1) return S_OK;

    PttUIBuffer *p_next_buffer = pttui_buffer_next_ne(current_buffer, PTTUI_BUFFER_INFO.tail);

    // 1. if no next: return
    if(!p_next_buffer) return S_OK;

    // 2. if no next-non-space-char: delete next char
    char *p_non_space_buf = NULL;
    error_code = pttui_next_non_space_char(p_next_buffer->buf, p_next_buffer->len_no_nl, &p_non_space_buf);
    if(error_code) return error_code;

    if(!p_non_space_buf) {
        error_code = _vedit3_action_delete_line_core(p_next_buffer);
        return error_code;
    }

    // 3. if no overflow
    int overflow = current_buffer->len_no_nl + p_next_buffer->len_no_nl - WRAPMARGIN;
    int non_space_offset = p_non_space_buf - p_next_buffer->buf;
    int len_word = 0;
    if(overflow < 0) {
        memcpy(current_buffer->buf + VEDIT3_EDITOR_STATUS.current_col, p_next_buffer->buf, p_next_buffer->len_no_nl);
        current_buffer->len_no_nl += p_next_buffer->len_no_nl;
        current_buffer->buf[current_buffer->len_no_nl] = 0;
        current_buffer->is_modified = true;

        error_code = _vedit3_action_delete_line_core(p_next_buffer);

        return error_code;
    }

    // 4. get first word
    error_code = pttui_first_word(p_non_space_buf, p_next_buffer->len_no_nl - non_space_offset, &len_word);
    if(error_code) return error_code;

    len_word += non_space_offset;

    // 4.2: if len + first_word >= WRAPMARGIN: no change
    if(current_buffer->len_no_nl + len_word >= WRAPMARGIN) return S_OK;

    // 4.3: concat first-word. shift next line.
    memcpy(current_buffer->buf + VEDIT3_EDITOR_STATUS.current_col, p_next_buffer->buf, len_word);
    current_buffer->len_no_nl += len_word;
    current_buffer->buf[current_buffer->len_no_nl] = 0;
    current_buffer->is_modified = true;

    error_code = pttui_next_non_space_char(p_next_buffer->buf + len_word, p_next_buffer->len_no_nl - len_word, &p_non_space_buf);
    if(error_code) return error_code;

    if(!p_non_space_buf) {
        error_code = _vedit3_action_delete_line_core(p_next_buffer);
        return error_code;
    }

    // shift next line.
    char *p_buf = p_next_buffer->buf;
    char *p_buf2 = p_non_space_buf;
    len_word = p_buf2 - p_buf;
    int n_shift = p_next_buffer->len_no_nl - len_word;
    for(int i = 0; *p_buf2 && i < n_shift; i++, p_buf++, p_buf2++) {
        *p_buf = *p_buf2;
    }
    p_next_buffer->len_no_nl -= len_word;
    p_next_buffer->is_modified = true;
    p_next_buffer->buf[p_next_buffer->len_no_nl] = 0;

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return S_OK;
}

Err
_vedit3_action_delete_line_core(PttUIBuffer *buffer)
{
    if (!VEDIT3_EDITOR_STATUS.is_own_wrlock_buffer_info) return S_ERR_EDIT_LOCK;

    buffer->is_to_delete = true;
    buffer->is_modified = true;

    // buffer after buffer
    for (PttUIBuffer *p_buffer2 = pttui_buffer_next_ne(buffer, PTTUI_BUFFER_INFO.tail); p_buffer2 && p_buffer2->content_type == buffer->content_type && p_buffer2->block_offset == buffer->block_offset && p_buffer2->comment_offset == buffer->comment_offset; p_buffer2->line_offset--, p_buffer2 = pttui_buffer_next_ne(p_buffer2, PTTUI_BUFFER_INFO.tail));

    // file-info
    Err error_code = S_OK;

    switch (buffer->content_type) {
    case PTTDB_CONTENT_TYPE_MAIN:
        error_code = file_info_decrease_main_content_line(&PTTUI_FILE_INFO, buffer->block_offset, buffer->file_offset);
        break;
    case PTTDB_CONTENT_TYPE_COMMENT_REPLY:
        error_code = file_info_decrease_comment_reply_line(&PTTUI_FILE_INFO, buffer->comment_offset, buffer->block_offset, buffer->file_offset);
        break;
    default:
        break;
    }

    // buffer-info
    PTTUI_BUFFER_INFO.n_to_delete++;


    // buffer-info head / tail
    PttUIBuffer *p_buffer = NULL;
    PttUIBuffer *p_buffer2 = NULL;
    if (PTTUI_BUFFER_INFO.head == buffer) {
        PTTUI_BUFFER_INFO.head = pttui_buffer_next_ne(buffer, PTTUI_BUFFER_INFO.tail);
        p_buffer = buffer;
        while(p_buffer != PTTUI_BUFFER_INFO.head) {
            p_buffer2 = p_buffer;
            p_buffer = p_buffer->next;

            p_buffer2->pre = NULL;
            p_buffer2->next = NULL;
            error_code = pttui_buffer_add_to_delete_buffer_list(p_buffer2, &PTTUI_BUFFER_INFO);
            if(error_code) break;
        }
        PTTUI_BUFFER_INFO.head->pre = NULL;
    }
    if(error_code) return error_code;

    if (PTTUI_BUFFER_INFO.tail == buffer) {
        PTTUI_BUFFER_INFO.tail = pttui_buffer_pre_ne(buffer, PTTUI_BUFFER_INFO.head);
        p_buffer = buffer;
        while(p_buffer != PTTUI_BUFFER_INFO.tail) {
            p_buffer2 = p_buffer;
            p_buffer = p_buffer->pre;

            p_buffer2->pre = NULL;
            p_buffer2->next = NULL;

            error_code = pttui_buffer_add_to_delete_buffer_list(p_buffer2, &PTTUI_BUFFER_INFO);
        }
        PTTUI_BUFFER_INFO.tail->next = NULL;
    }
    if(error_code) return error_code;

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return S_OK;
}

Err
_vedit3_action_save()
{
    return S_OK;
}