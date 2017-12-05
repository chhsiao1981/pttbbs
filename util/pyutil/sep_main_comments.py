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
# Format:
#      comments:
#          version                   (unsigned char, 1 byte)
#          n-comments                (int, 4 bytes)
#          comments                  (comment[])
#
#      For each comment:
#          length                    (unsigned char, 1 byte) (not include this byte)
#          the_type                  (unsigned char, 1 byte)
#          timestamp                 (unsigned long long, 8 bytes)
#          len-username              (unsigned char, 1 byte)
#          username                  (unsigned char[])
#          comment-content           (unsigned char[])
#
#      comment-reply:
#          version                   (unsigned char, 1 byte)
#          n-comment-reply           (int, 4 bytes)
#          each-comment-reply        (comment-reply[])
#
#      For each comment-reply:
#          length-in-byte            (unsigned short, 2 bytes) (not include these 2 bytes)
#          comment_id                (int, 4 bytes)
#          timestamp                 (unsigned long long, 8 bytes)
#          comments                  (unsigned char[])
#
#     comment-reply.idx:
#          version                   (unsigned char, 1 byte)
#          n-comment-reply           (int, 4 bytes)
#          each-comment-reply.idx    (comment-reply.idx[])
#
#     For each comment-reply.idx:
#          comment-id                (int, 4 bytes)
#          offset                    (unsigned int, 4 bytes)
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
import struct
from datetime import datetime, timedelta
import calendar

S_ERR = 1

VERSION = 1

STATE_COMMENT_INIT = 1
STATE_COMMENT_COMMENT = 2
STATE_COMMENT_COMMENT_REPLY = 3

RECTYPE_NONE = -1
RECTYPE_GOOD = 0
RECTYPE_BAD = 1
RECTYPE_ARROW = 2
RECTYPE_SIZE = 3
RECTYPE_FORWARD = 4  # XXX HACK that RECTYPE_SIZE = 3


def timestamp_to_datetime(the_timestamp, tz=8):
    return datetime.utcfromtimestamp(the_timestamp / 1000 + tz * 3600)


def datetime_to_timestamp(the_datetime, tz=8):
    return (calendar.timegm(the_datetime.timetuple()) - tz * 3600) * 1000


def Big5(content):
    """Try to encode to big5 from unicode

    Args:
        content (str): content with unicode (const str)

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
    the_timestamp = _parse_main_content_timestamp(main, origin, the_from)

    return main + b'\r\n' + origin + b'\r\n' + the_from, the_timestamp


def _parse_main_content_timestamp(content, origin, the_from):
    the_from_list = the_from.split(b'\n')
    the_from_list = [each_from.strip() for each_from in the_from_list]
    link_str = Big5('※ 文章網址')

    for each_from in the_from_list:
        re_match = re.match(b'^' + link_str + b':\s*(.*?)\s*$', each_from)
        if re_match:
            the_link_str = re_match.group(1).strip().decode('utf-8')
            the_link_list = the_link_str.split('/')
            the_filename = the_link_list[-1]
            the_filename_list = the_filename.split('.')
            the_timestamp = int(the_filename_list[1]) * 1000
            return the_timestamp

    content_list = content.split(b'\n')
    content_list = content_list[:5]
    content_list = [each_content.strip() for each_content in content_list]


    time_str = Big5('時間')
    for each_content in content_list:
        re_match = re.match(b'^' + time_str + b':\s*(.*?)\s*$', each_content)
        if re_match:
            the_datetime_str = re.sub(b'\s+', b' ', re_match.group(1)).strip().decode('utf-8')
            the_datetime = datetime.strptime(the_datetime_str, '%c')
            return datetime_to_timestamp(the_datetime)

    logging.error('unable to parse main-content time')

    return datetime_to_timestamp(datetime(year=1998, month=1, day=1))


def integrate_comment_block(first_comment, the_rest_comments, previous_post_time):
    all_comments = first_comment + b'\r\n' + the_rest_comments

    # get comment_list without \r\n and without tail empty 2 lines
    comment_list = all_comments.split(b'\n')
    comment_list = [each_comment[:-1] for each_comment in comment_list]
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
    state = STATE_COMMENT_INIT

    each_comment_reply_list = []
    for each_comment in comment_list:
        error, (comment, username, post_time, the_type) = _parse_comment(each_comment, previous_post_time)

        # get comment
        if error is None:
            if each_comment_reply_list:
                each_comment_reply = b'\r\n'.join(each_comment_reply_list)
                comment_reply.append({'reply': each_comment_reply, 'comment_id': max(len(comments) - 1, 0), 'ts': 0})
                each_comment_reply_list = []

            comments.append({'comment': comment, 'username': username, 'ts': post_time, 'the_type': the_type})
            previous_post_time = post_time

            state = STATE_COMMENT_COMMENT
            continue

        # consider as comment-reply
        each_comment_reply_list.append(each_comment)
        state = STATE_COMMENT_COMMENT_REPLY

    return None, (comments, comment_reply)


def _parse_comment(comment, previous_post_time):
    """Summary

    Args:
        comment (TYPE): Description

    Returns:
        error, (bytes, bytes, int, int): error-code, (comment, username, post_time, the_type)
    """

    error, (comment_content, username, post_time, the_type) = _parse_comment_recommend(comment, previous_post_time)
    if error is None:
        logging.warning('comment_content: %s post_time: %s', comment_content, timestamp_to_datetime(post_time))
        return None, (comment_content, username, post_time, the_type)

    error, (comment_content, username, post_time, the_type) = _parse_comment_boo(comment, previous_post_time)
    if error is None:
        logging.warning('comment_content: %s post_time: %s', comment_content, timestamp_to_datetime(post_time))
        return None, (comment_content, username, post_time, the_type)

    error, (comment_content, username, post_time, the_type) = _parse_comment_comment(comment, previous_post_time)
    if error is None:
        logging.warning('comment_content: %s post_time: %s', comment_content, timestamp_to_datetime(post_time))
        return None, (comment_content, username, post_time, the_type)

    error, (comment_content, username, post_time, the_type) = _parse_comment_forward(comment, previous_post_time)
    if error is None:
        logging.warning('comment_content: %s post_time: %s', comment_content, timestamp_to_datetime(post_time))
        return None, (comment_content, username, post_time, the_type)

    return S_ERR, (b'', b'', 0, 0)


def _parse_comment_recommend(comment, previous_post_time):
    """Summary

    Args:
        comment (TYPE): Description
        previous_post_time (TYPE): Description

    Returns:
        error, (bytes, bytes, int, int): error-code, (comment, username, post_time, the_type)
    """
    bright_white = Ctrl(b'\[1;37m')
    yellow = Ctrl(b'\[33m')
    color_reset = Ctrl(b'\[m')

    username = b'[0-9A-Za-z]+'

    recommend = Big5('推')
    regex = bright_white + recommend + b' ' + yellow + b'(' + username + b')' + color_reset + yellow + b':(.*)'

    re_match = re.match(regex, comment)
    if re_match is None:
        return S_ERR, (b'', b'', 0, 0)

    username = re_match.group(1)
    the_rest_comment = re_match.group(2)
    post_time = _parse_post_time(the_rest_comment, previous_post_time)

    return None, (comment, username, post_time, RECTYPE_GOOD)


def _parse_comment_boo(comment, previous_post_time):
    """Summary

    Args:
        comment (TYPE): Description
        previous_post_time (TYPE): Description

    Returns:
        error, (bytes, bytes, int, int): error-code, (comment, username, post_time, the_type)
    """
    yellow = Ctrl(b'\[33m')
    color_reset = Ctrl(b'\[m')

    username = b'[0-9A-Za-z]+'

    bright_red = Ctrl(b'\[1;31m')
    boo = Big5('噓')
    regex = bright_red + boo + b' ' + yellow + b'(' + username + b')' + color_reset + yellow + b':(.*)'

    re_match = re.match(regex, comment)
    if re_match is None:
        return S_ERR, (b'', b'', 0, 0)

    username = re_match.group(1)
    the_rest_comment = re_match.group(2)
    post_time = _parse_post_time(the_rest_comment, previous_post_time)

    return None, (comment, username, post_time, RECTYPE_BAD)


def _parse_comment_comment(comment, previous_post_time):
    """Summary
    
    Args:
        comment (TYPE): Description
        previous_post_time (TYPE): Description
    
    Returns:
        TYPE: Description
    """
    bright_red = Ctrl(b'\[1;31m')
    yellow = Ctrl(b'\[33m')
    color_reset = Ctrl(b'\[m')

    username = b'[0-9A-Za-z]+'
    comment = Big5('→')
    regex = bright_red + comment + b' ' + yellow + b'(' + username + b')' + color_reset + yellow + b':(.*)'

    re_match = re.match(regex, comment)
    if re_match is None:
        return S_ERR, (b'', b'', 0, 0)

    username = re_match.group(1)
    the_rest_comment = re_match.group(2)
    post_time = _parse_post_time(the_rest_comment, previous_post_time)

    return None, (comment, username, post_time, RECTYPE_ARROW)


def _parse_comment_forward(comment, previous_post_time):
    """Summary
    
    Args:
        comment (TYPE): Description
        previous_post_time (TYPE): Description
    
    Returns:
        TYPE: Description
    """
    bright_green = Ctrl(b'\[1;32m')
    green = Ctrl(b'\[0;32m')
    forward_to = Big5('轉錄至看板')
    username = b'[0-9A-Za-z]+'
    regex = Big5('※') + b' ' + bright_green + b'(' + username + b')' + green + b':' + forward_to + b'(.*)'

    re_match = re.match(regex, comment)
    if re_match is None:
        return S_ERR, (b'', b'', 0, 0)

    username = re_match.group(1)
    the_rest_comment = re_match.group(2)
    post_time = _parse_post_time(the_rest_comment, previous_post_time)

    return None, (comment, username, post_time, RECTYPE_FORWARD)


def _parse_post_time(the_rest_comment, previous_post_time):
    """Summary
    
    Args:
        the_rest_comment (TYPE): Description
        previous_post_time (TYPE): Description
    
    Returns:
        TYPE: Description
    """
    previous_datetime = timestamp_to_datetime(previous_post_time)

    # MM/DD hh:mm
    the_match = re.search(b'(\d{1,2})/(\d{1,2}) (\d{1,2}):(\d{1,2})\s*$', the_rest_comment)
    if the_match:
        month = int(the_match.group(1))
        day = int(the_match.group(2))
        hour = int(the_match.group(3))
        minute = int(the_match.group(4))
        return _parse_post_time_core(previous_datetime.year, month, day, hour, minute, previous_post_time)

    # MM/DD hh:
    the_match = re.search(b'(\d{1,2})/(\d{1,2}) (\d{1,2}):?\s*$', the_rest_comment)
    if the_match:
        month = int(the_match.group(1))
        day = int(the_match.group(2))
        hour = int(the_match.group(3))
        minute = previous_datetime.minute if month == previous_datetime.month and day == previous_datetime.day and hour == previous_datetime.hour else 0
        return _parse_post_time_core(previous_datetime.year, month, day, hour, minute, previous_post_time)

    # MM/DD:
    the_match = re.search(b'(\d{1,2})/(\d{1,2})\s*$', the_rest_comment)
    if the_match:
        month = int(the_match.group(1))
        day = int(the_match.group(2))
        hour = previous_datetime.hour if month == previous_datetime.month and day == previous_datetime.day else 0
        minute = previous_datetime.minute if month == previous_datetime.month and day == previous_datetime.day and hour == previous_datetime.hour else 0
        return _parse_post_time_core(previous_datetime.year, month, day, hour, minute, previous_post_time)

    # MM:
    the_match = re.search(b'(\d{1,2})/?\s*$', the_rest_comment)
    if the_match:
        month = int(the_match.group(1))
        day = previous_datetime.day if month == previous_datetime.month else 1
        hour = previous_datetime.hour if month == previous_datetime.month and day == previous_datetime.day else 0
        minute = previous_datetime.minute if month == previous_datetime.month and day == previous_datetime.day and hour == previous_datetime.hour else 0
        return _parse_post_time_core(previous_datetime.year, month, day, hour, minute, previous_post_time)

    return previous_post_time + 1


def _parse_post_time_core(year, month, day, hour, minute, previous_timestamp):
    """Summary
    
    Args:
        year (TYPE): Description
        month (TYPE): Description
        day (TYPE): Description
        hour (TYPE): Description
        minute (TYPE): Description
        previous_timestamp (TYPE): Description
    
    Returns:
        TYPE: Description
    """
    logging.warning('year: %s month: %s day: %s hour: %s minute: %s previous_timestamp: %s', year, month, day, hour, minute, previous_timestamp)
    error = None
    try:
        the_datetime = datetime(year=year, month=month, day=day, hour=hour, minute=minute, second=59)
    except Exception as e:
        logging.error('unable to do datetime: year: %s month: %s day: %s', year, month, day)
        error = S_ERR

    if error and month == 2 and day == 29:
        return _parse_post_time_core(year + 1, month, day, hour, minute, previous_timestamp)

    the_timestamp = datetime_to_timestamp(the_datetime)

    previous_datetime = timestamp_to_datetime(previous_timestamp)
    previous_datetime = previous_datetime + timedelta(microseconds=previous_datetime.microsecond)
    previous_timestamp_by_second = datetime_to_timestamp(previous_datetime)
    logging.warning('previous_datetime: %s the_datetime: %s the_timestamp: %s previous_timestamp_by_second: %s the_timestamp > previous_timestamp_by_second: %s', previous_datetime, the_datetime, the_timestamp, previous_timestamp_by_second, the_timestamp > previous_timestamp_by_second)
    if the_timestamp > previous_timestamp_by_second:
        return the_timestamp
    elif the_timestamp == previous_timestamp_by_second:
        return previous_timestamp + 1

    return _parse_post_time_core(year + 1, month, day, hour, minute, previous_timestamp)


def comments_to_file(comments):
    """Compile comments to file

       comments:
           version                   (unsigned char, 1 byte)
           n-comments                (int, 4 bytes)
           comments                  (comment[])

    Args:
        comments (list): [{'comment', 'username', 'ts', 'the_type'}]

    Returns:
        bytes: compiled content-in-file
    """
    len_comments = len(comments)
    b_version_len_comments = struct.pack('<Bi', VERSION, len_comments)

    results = b_version_len_comments
    for each_comment in comments:
        results += _comment_to_file(each_comment)

    return results


def _comment_to_file(comment):
    """comple comment to stored in file

       comment:
           length                    (unsigned char, 1 byte)
           the_type                  (unsigned char, 1 byte)
           timestamp                 (unsigned long long, 8 bytes)
           len-username              (unsigned char, 1 byte)
           username                  (unsigned char[])
           comment-content           (unsigned char[])

    Args:
        comment (dict): {comment, username, ts, the_type}

    Returns:
        bytes: compiled content-in-file
    """

    comment_content = comment.get('comment', b'')
    username = comment.get('username', b'')
    ts = comment.get('ts', 0)
    the_type = comment.get('the_type', RECTYPE_NONE)
    len_username = len(username)

    b_the_type_ts_len_username = struct.pack('<BQB', the_type, ts, len_username)
    the_length = len(b_the_type_ts_len_username) + len(username) + len(comment_content)
    b_length = struct.pack('<B', the_length)
    comment_on_file = b_length + b_the_type_ts_len_username + username + comment_content

    return comment_on_file


def comment_reply_to_file(comment_reply):
    """Compile lists of comment-reply to file (comment-reply-on-file and comment-reply-idx-on-file)

       comment-reply:
            version                   (unsigned char, 1 byte)
            n-comment-reply           (int, 4 bytes)
            each-comment-reply        (comment-reply[])

       For each comment-reply:
            length-in-byte            (unsigned short, 2 bytes) (not include these 2 bytes)
            comment_id                (int, 4 bytes)
            timestamp                 (unsigned long long, 8 bytes)
            comments                  (unsigned char[])

       comment-reply.idx:
            version                   (unsigned char, 1 byte)
            n-comment-reply           (int, 4 bytes)
            each-comment-reply.idx    (comment-reply.idx[])

       For each comment-reply.idx:
            comment-id                (int, 4 bytes)
            offset                    (unsigned int, 4 bytes)

    Args:
        comment_reply (list): [{reply, comment_id, ts}]

    Returns:
        (bytes, bytes): comment-reply-on-file, comment-reply-idx-on-flie
    """
    len_comment_reply = len(comment_reply)
    b_version_len_comment_reply = struct.pack('<Bi', VERSION, len_comment_reply)
    results_comment_reply = b_version_len_comment_reply
    results_idx = b_version_len_comment_reply
    for each_comment_reply in comment_reply:
        each_comment_reply_in_file, each_comment_reply_idx_in_file = _comment_reply_to_file(each_comment_reply, len(results_comment_reply))
        results_comment_reply += each_comment_reply_in_file
        results_idx += each_comment_reply_idx_in_file

    return results_comment_reply, results_idx


def _comment_reply_to_file(comment_reply, len_results_comment_reply):
    """compile comment_reply to stored in file

    Args:
        comment_reply (dict): {reply, comment_id, ts}

    Returns:
        bytes, bytes: comment-reply-on-file, comment-reply-idx-on-file
    """
    reply = comment_reply.get('reply', b'')
    comment_id = comment_reply.get('comment_id', 0)
    ts = comment_reply.get('ts', 0)

    b_comment_id_ts = struct.pack('<iQ', comment_id, ts)
    the_length = len(reply) + len(b_comment_id_ts)
    b_length = struct.pack('<H', the_length)
    comment_reply_on_file = b_length + b_comment_id_ts + reply

    comment_reply_idx_on_file = struct.pack('<iI', comment_id, len_results_comment_reply)

    return comment_reply_on_file, comment_reply_idx_on_file


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


def _main():
    filename = sys.argv[1]

    with open(filename, 'rb') as f:
        content = f.read()

    error, (the_main, origin, the_from, first_comment, the_rest_comments) = sep_main_comments(content)

    if error:
        logging.error('unable to sep main-comments, assuming no comments')

    main_content, the_timestamp = integrate_main_content(the_main, origin, the_from)

    logging.warning('guessed create timestamp: %s', timestamp_to_datetime(the_timestamp))

    error, (comments, comment_reply) = integrate_comment_block(first_comment, the_rest_comments, the_timestamp)
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
