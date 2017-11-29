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

STATE_COMMENT_INIT = 1
STATE_COMMENT_COMMENT = 2
STATE_COMMENT_COMMENT_REPLY = 3

S_ERR = 1


def Big5(content):
    """Try to encode to big5 from utf-content

    Args:
        content (str): content with utf-8 encoding

    Returns:
        bytes: content with big-5 encoding
    """
    return content.encode('big5')


def Ctrl(content):
    """Try to set as control-content

    Args:
        content (bytes): content to be prefixed as control

    Returns:
        str: prefixed with control-char
    """
    return b'\x1b' + content


def sep_main_comments(content):
    """Try to separate among main-content, origin, from, and comments
       assuming with comments first
       then parse without comments

    Args:
        content (bytes): single-file-content

    Returns:
        Error, (bytes, bytes, bytes, bytes, bytes): error, (the_main, origin, the_from, first_comment, the_rest_comments)
    """
    error, (the_main, origin, the_from, first_comment, the_rest_comments) = _sep_main_comments_assuming_comments(content)
    if error is None:
        return None, (the_main, origin, the_from, first_comment, the_rest_comments)

    logging.error('unable to sep main-comments, assuming no comments')
    error, (the_main, origin, the_from) = _sep_main(content)
    if error is None:
        return None, (the_main, origin, the_from, first_comment, the_rest_comments)

    logging.error('unable to sep main-comments')
    return S_ERR, (b'', b'', b'', b'', b'')


def integrate_main_content(main, origin, the_from):
    """integrate main, origin, from to main_content

    Args:
        main (bytes): main
        origin (bytes): origin
        the_from (bytes): from

    Returns:
        byte: main_content
    """
    return main + b'\r\n' + origin + b'\r\n' + the_from


def integrate_comment_block(first_comment, the_rest_comments):
    all_comments = first_comment + b'\r\n' + the_rest_comments

    # get comment_list without \r\n and without tail empty 2 lines
    comment_list = all_comments.split(b'\n')
    comment_list = [each_comment[:-1] for each_comment in comment_list]
    if not comment_list[-1]:
        comment_list = comment_list[:-1]
    if not comment_list[-1]:
        comment_list = comment_list[:-1]
    if not comment_list[-1]:
        comment_list = comment_list[:-1]

    # state-machine for comment:
    #   if begin with comment: record username, posttime, comments, finish previous comment-reply, state as comment-state
    #   else:
    #     comment-reply append, comment-state as comment-reply
    comments = []
    comment_reply = []
    previous_comment_post_time = 0
    state = STATE_COMMENT_INIT

    each_comment_reply_list = []
    for each_comment in comment_list:
        error, (comment, username, post_time, the_type) = _parse_comment(each_comment)

        # get comment
        if error is None:
            comments.append({'comment': comment, 'username': username, 'ts': post_time, 'the_type': the_type})

            if each_comment_reply_list:
                each_comment_reply = b'\r\n'.join(each_comment_reply_list)
                comment_reply.append({'reply': each_comment_reply, 'comment_id': len(comments) - 1, 'ts': 0})
                each_comment_reply_list = []

            state = STATE_COMMENT_COMMENT
            continue

        # consider as comment-reply
        each_comment_reply_list.append(each_comment)
        state = STATE_COMMENT_COMMENT_REPLY

    return None, (comments, comment_reply)


def comments_to_file(comments):
    """Compile comments to file

    Args:
        comments (list): [{'comment', 'username', 'ts', 'the_type'}]

    Returns:
        bytes: compiled content-in-file
    """
    return b''


def comment_reply_to_file(comment_reply):
    """Compile lists of comment-reply to file (comment-reply-on-file and comment-reply-idx-on-file)

    Args:
        comment_reply (list): [{reply, comment_id, ts}]

    Returns:
        (bytes, bytes): comment-reply-on-file, comment-reply-idx-on-flie
    """
    return b'', b''


def _sep_main_comments_assuming_comments(content):
    """Try to separate among main-content, origin, from, and comments
        (正文)\r\n※ 發信站:([^換行]+)\r\n(※ 文章網址/※ 編輯/※ 轉錄者)\r\n(推/噓/→/※ username:轉錄至看板)(.*)
        group1: main-content (no last \r\n)
        group2: 發信站 content (no \r\n)
        group3: 文章網址 / 轉錄者 / ... (+ last \r\n)
        group4: 文章網址 / 轉錄者 / ... (no last \r\n)
        group5: 第 0 個 comment-header
        group6: 第 0 個 comment-rest (no \r\n)
        group7: the rest of the comments (no last \r\n)

    Args:
        content (bytes): original content

    Returns:
        Error, (bytes, bytes, bytes, bytes, bytes): error-code, (main, origin, from, first-comment, rest-comments)
    """

    main_wildcard = b'.*'

    origin_head = Big5('※ 發信站:')
    origin_wildcard = b'[^\r\n]+'

    from_wildcard = b'.*?'

    bright_white = Ctrl(b'\[1;37m')
    yellow = Ctrl(b'\[33m')
    color_reset = Ctrl(b'\[m')

    username = b'[0-9A-Za-z]+'

    recommend = Big5('推')
    recommend_wildcard = bright_white + recommend + b' ' + yellow + username + color_reset + yellow + b':'

    bright_red = Ctrl(b'\[1;31m')
    boo = Big5('噓')
    boo_wildcard = bright_red + boo + b' ' + yellow + username + color_reset + yellow + b':'

    comment = Big5('→')
    comment_wildcard = bright_red + comment + b' ' + yellow + username + color_reset + yellow + b':'

    bright_green = Ctrl(b'\[1;32m')
    green = Ctrl(b'\[0;32m')
    forward_to = Big5('轉錄至看板')
    forward_wildcard = Big5('※') + b' ' + bright_green + username + green + b':' + forward_to

    first_comment_head_wildcard = recommend_wildcard + b'|' + boo_wildcard + b'|' + comment_wildcard + b'|' + forward_wildcard
    first_comment_wildcard = b'.*?'

    the_rest_comments_wildcard = b'.*'

    regex = b'(' + main_wildcard + b')\r\n' + \
        origin_head + b'(' + origin_wildcard + b')\r\n' + \
        b'((' + from_wildcard + b')\r\n)?' + \
        b'(' + first_comment_head_wildcard + b')(' + first_comment_wildcard + b')\r\n' + \
        b'(' + the_rest_comments_wildcard + b')'

    re_match = re.match(regex, content, re.M | re.S)

    if re_match is None:
        return S_ERR, (b'', b'', b'', b'', b'')

    the_main = re_match.group(1)
    origin = origin_head + re_match.group(2)
    the_from = re_match.group(4)
    first_comment = re_match.group(5) + re_match.group(6)
    the_rest_comments = re_match.group(7)

    return None, (the_main, origin, the_from, first_comment, the_rest_comments)


def _sep_main(content):
    """Try to separate among main-content, origin, and from
        (正文)\r\n※ 發信站:([^換行]+)\r\n(※ 文章網址/※ 編輯/※ 轉錄者)\r\n
        group1: main (no last \r\n)
        group2: 發信站 content (no \r\n)
        group3: 文章網址 / 轉錄者 / ... (+ last \r\n)
        group4: 文章網址 / 轉錄者 / ... (no last \r\n)

    Args:
        content (str): original content

    Returns:
        Error, (str, str, str): error-code, (main, origin, from)
    """
    main_wildcard = b'.*'

    origin_head = Big5('※ 發信站:')
    origin_wildcard = b'[^\r\n]+'

    from_wildcard = b'.*?'

    regex = b'(' + main_wildcard + b')\r\n' + \
        origin_head + b'(' + origin_wildcard + b')\r\n' + \
        b'((' + from_wildcard + b')\r\n)?'

    re_match = re.match(regex, content, re.M | re.S)

    if re_match is None:
        return S_ERR, (b'', b'', b'')

    the_main = re_match.group(1)
    origin = origin_head + re_match.group(2)
    the_from = re_match.group(4)

    return None, (the_main, origin, the_from)


def _parse_comment(comment):
    """Parse comment

    Args:
        comment (str): one-line comment

    Returns:
        Error, (str, str, str, str): error-code, (comment, username, post-time-in-millisecond, the_type)
    """
    return None, (b'', b'', b'', b'')


def _main():
    filename = sys.argv[1]

    with open(filename, 'rb') as f:
        content = f.read()

    error, (the_main, origin, the_from, first_comment, the_rest_comments) = sep_main_comments(content)

    if error:
        logging.error('unable to sep main-comments, assuming no comments')

    main_content = integrate_main_content(the_main, origin, the_from)

    error, (comments, comment_reply) = integrate_comment_block(first_comment, the_rest_comments)
    if error:
        logging.error('unable to integrate comment block')

    comments_on_file = comments_to_file(comments)

    comment_reply_on_file, comment_reply_idx_on_file = comment_reply_to_file(comment_reply)

    out_filename = filename + '.out0.main'
    with open(out_filename, 'wb') as f:
        f.write(the_main)
        f.write(b'\r\n')

    out_filename = filename + '.out1.origin'
    with open(out_filename, 'wb') as f:
        f.write(origin)
        f.write(b'\r\n')

    out_filename = filename + '.out2.from'
    with open(out_filename, 'wb') as f:
        f.write(the_from)
        f.write(b'\r\n')

    out_filename = filename + '.out3.comment0'
    with open(out_filename, 'wb') as f:
        f.write(first_comment)
        f.write(b'\r\n')

    out_filename = filename + '.out4.rest_comments'
    with open(out_filename, 'wb') as f:
        f.write(the_rest_comments)

    out_filename = filename + '.out5.main_content'
    with open(out_filename, 'wb') as f:
        f.write(main_content)

    out_filename = filename + '.out6.comments'
    with open(out_filename, 'wb') as f:
        f.write(comments_on_file)

    out_filename = filename + '.out7.comment_reply'
    with open(out_filename, 'wb') as f:
        f.write(comment_reply_on_file)

    out_filename = filename + '.out8.comment_reply.idx'
    with open(out_filename, 'wb') as f:
        f.write(comment_reply_idx_on_file)


if __name__ == '__main__':
    _main()
