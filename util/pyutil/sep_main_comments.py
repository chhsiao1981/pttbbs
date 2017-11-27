#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# https://github.com/ptt/pttbbs/issues/31
#
# Goal: 根據目前所看到的文章們.
#       contents 分成以下幾個部分:
#        1. 本文
#        2. comments
#        3. comment-reply
#       其中:
#        1. 本文是指 "最後一個發信站之前" 都是本文
#        2. comments: "推 / 噓 / -> / 轉錄至"
#        3. comment-reply: 在 comments 之後. 有 poster 所做的以 edit 的方式做 reply
#
# 方式: 1. 找到最後一個發信站/from. 之前都是算本文. 之後算 comment-block
#      2. 在本文裡分出 main, origin, from
#      3. 在 comment-block 裡分出 comments 和 comment-reply
#
#
# USAGE (python3): sep_main_content_comments.py [in_filename]
#
# Generate: .main0: main-content (no last \r\n)
#           .origin: 發信站 (no \r\n)
#           .from: 文章網址 / 轉錄者 / ... (no \r\n)
#           .comment0: 第 0 個 comment (no \r\n)
#           .rest_comments: the rest of the comments (no last \r\n)
#           .main: main0 + origin + from
#           .comments: n_lines,strlen + str (include \0, no \r\n)
#           .comment_reply: size + idx + str (include \0)
# TODO: 1. 找出 main-content 裡有推 / 噓 / -> / 轉錄至 的文章們
#          確認如此的 context 是否符合預期.
#       2. UI: pmore 改成如果讀到 single-file: 先轉成 main-content / comments / comment-reply, 再換成新版 pmore.
#       3. UI: 在用 E 時. 改為先選 block.
#              然後如果是在 main-content 區.
#              則只能編輯 main-content 區.
#              如果是在 comment 區.
#              只會選當下的 comment 做 reply.
#              edit 完以後儲存以後.
#              回到選 block 的功能.
#              然後再按 esc 離開 edit.

import sys
import logging
import re

S_OK = 0
S_ERR = 1


def Big5(content):
    """Try to encode to big5 from utf-content

    Args:
        content (str): content with utf-8 encoding

    Returns:
        str: content with big-5 encoding
    """
    return content.decode('utf-8').encode('big5')


def Ctrl(content):
    """Try to set as control-content

    Args:
        content (str): content to be prefixed as control

    Returns:
        str: prefixed with control-char
    """
    return '\x1b' + content


def sep_main_comments(content):
    '''Try to separate among main-content, origin, from, and comments
        (正文)\r\n※ 發信站:([^換行]+)\r\n(※ 文章網址/※ 編輯/※ 轉錄者)\r\n(推/噓/→/※ username:轉錄至看板)(.*)
        group1: main-content (no last \r\n)
        group2: 發信站 content (no \r\n)
        group3: 文章網址 / 轉錄者 / ... (+ last \r\n)
        group4: 文章網址 / 轉錄者 / ... (no last \r\n)
        group5: 第 0 個 comment-header
        group6: 第 0 個 comment-rest (no \r\n)
        group7: the rest of the comments (no last \r\n)

    Args:
        content (str): original content

    Returns:
        Error, (str, str, str, str, str): error-code, (main-content, origin, from, first-comment, rest-comments)
    '''

    main_wildcard = '.*'

    origin_head = Big5('※ 發信站:')
    origin_wildcard = '[^\r\n]+'

    from_wildcard = '.*?'

    bright_white = Ctrl('\[1;37m')
    yellow = Ctrl('\[33m')
    color_reset = Ctrl('\[m')

    username = '[0-9A-Za-z]+'

    recommend = Big5('推')
    recommend_wildcard = bright_white + recommend + ' ' + yellow + username + color_reset + yellow + ':'

    bright_red = Ctrl('\[1;31m')
    boo = Big5('噓')
    boo_wildcard = bright_red + boo + ' ' + yellow + username + color_reset + yellow + ':'

    comment = Big5('→')
    comment_wildcard = bright_red + comment + ' ' + yellow + username + color_reset + yellow + ':'

    bright_green = Ctrl('\[1;32m')
    green = Ctrl('\[0;32m')
    forward_to = Big5('轉錄至看板')
    forward_wildcard = Big5('※') + ' ' + bright_green + username + green + ':' + forward_to

    first_comment_head_wildcard = recommend_wildcard + '|' + boo_wildcard + '|' + comment_wildcard + '|' + forward_wildcard
    first_comment_wildcard = '.*?'

    the_rest_comments_wildcard = '.*'

    regex = '(' + main_wildcard + ')\r\n' + \
        origin_head + '(' + origin_wildcard + ')\r\n' + \
        '((' + from_wildcard + ')\r\n)?' + \
        '(' + first_comment_head_wildcard + ')(' + first_comment_wildcard + ')\r\n' + \
        '(' + the_rest_comments_wildcard + ')'

    re_match = re.match(regex, content, re.M | re.S)

    if re_match is None:
        return S_ERR, ('', '', '', '', '')

    main_content = re_match.group(1)
    origin = origin_head + re_match.group(2)
    the_from = re_match.group(4)
    first_comment = re_match.group(5) + re_match.group(6)
    the_rest_comments = re_match.group(7)

    return S_OK, (main_content, origin, the_from, first_comment, the_rest_comments)


def sep_main(content):
    """Try to separate among main-content, origin, and from
        (正文)\r\n※ 發信站:([^換行]+)\r\n(※ 文章網址/※ 編輯/※ 轉錄者)\r\n
        group1: main-content (no last \r\n)
        group2: 發信站 content (no \r\n)
        group3: 文章網址 / 轉錄者 / ... (+ last \r\n)
        group4: 文章網址 / 轉錄者 / ... (no last \r\n)

    Args:
        content (str): original content

    Returns:
        Error, (str, str, str): error-code, (main-content, origin, from)
    """
    main_wildcard = '.*'

    origin_head = Big5('※ 發信站:')
    origin_wildcard = '[^\r\n]+'

    from_wildcard = '.*?'

    regex = '(' + main_wildcard + ')\r\n' + \
        origin_head + '(' + origin_wildcard + ')\r\n' + \
        '((' + from_wildcard + ')\r\n)?'

    re_match = re.match(regex, content, re.M | re.S)

    if re_match is None:
        return S_ERR, ('', '', '')

    main_content = re_match.group(1)
    origin = origin_head + re_match.group(2)
    the_from = re_match.group(4)

    return S_OK, (main_content, origin, the_from)


def _main():
    filename = sys.argv[1]

    with open(filename, 'r') as f:
        content = f.read()

    error, (main_content, origin, the_from, first_comment, the_rest_comments) = sep_main_comments(content)

    if error:
        logging.error('unable to sep main-comments, assuming no comments')
        error, (main_content, origin, the_from) = sep_main(content)

        if error:
            logging.error('unable to sep main')
            main_content = content

    out_filename = filename + '.out0.main'
    with open(out_filename, 'w') as f:
        f.write(main_content)
        f.write('\r\n')

    out_filename = filename + '.out1.origin'
    with open(out_filename, 'w') as f:
        f.write(origin)
        f.write('\r\n')

    out_filename = filename + '.out2.from'
    with open(out_filename, 'w') as f:
        f.write(the_from)
        f.write('\r\n')

    out_filename = filename + '.out3.comment0'
    with open(out_filename, 'w') as f:
        f.write(first_comment)
        f.write('\r\n')

    out_filename = filename + '.out4.comments'
    with open(out_filename, 'w') as f:
        f.write(the_rest_comments)


if __name__ == '__main__':
    _main()
