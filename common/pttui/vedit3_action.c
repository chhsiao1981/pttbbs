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

        switch (ch) {
        case KEY_F10:
        case Ctrl('X'): // save and exit
            *is_end = true;
            break;
        case KEY_F5:    // goto-line
            break;

        case KEY_F8:    // users
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
        case KEY_F1:
        case Ctrl('Z'): // help
            error_code = _vedit3_action_show_help();
            break;

        case Ctrl('L'); // redraw
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
    return S_OK;    
}


Err
_vedit3_action_toggle_ansi()
{
    VEDIT3_EDITOR_STATUS.is_ansi ^= 1;

    // XXX block selection

    VEDIT3_EDITOR_STATUS.is_redraw_everything = true;

    return S_OK;
}