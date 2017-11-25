#!/usr/bin/env python
# -*- coding: utf-8 -*-
# USAGE: sep_main_content_comments.py [in_filename]
#
# Generate: .main: main-content (no last \r\n)
#           .origin: 發信站 (no \r\n)
#           .from: 文章網址 / 轉錄者 / ... (no \r\n)
#           .comment0: 第 0 個 comment (no \r\n)
#           .comments: the rest of the comments (no last \r\n)

import sys
import logging
import re

S_OK = 0
S_ERR = 1


def sep_main_comments(content):
    '''
        (正文)\r\n※ 發信站:([^換行]+)\r\n(※ 文章網址/※ 編輯/※ 轉錄者)\n\n(推/噓/→/※ username:轉錄至看板)(.*)
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
    re_match = re.match(r'(.*)\r\n\xa1\xb0 \xb5o\xabH\xaf\xb8:([^\r\n]+)\r\n((.*?)\r\n)?(\x1b\[1;37m\xb1\xc0|\x1b\[1;31m\xbcN|\x1b\[1;31m\xa1\xf7|\xa1\xb0 \x1b\[1;32m[0-9A-Za-z]+\x1b\[0;32m:\xc2\xe0\xbf\xfd\xa6\xdc\xac\xdd\xaaO)(.*?)\r\n(.*)', content, re.M|re.S)

    if re_match is None:
        return S_ERR, (None, None, None, None, None)

    main_content = re_match.group(1)
    origin = '\xa1\xb0 \xb5o\xabH\xaf\xb8:' + re_match.group(2)
    the_from = re_match.group(4)
    first_comment = re_match.group(5) + re_match.group(6)
    the_rest_comments = re_match.group(7)

    return S_OK, (main_content, origin, the_from, first_comment, the_rest_comments)


def sep_main(content):
    re_match = re.match(r'(.*)\r\n\xa1\xb0 \xb5o\xabH\xaf\xb8:([^\r\n]+)\r\n((.*?)\r\n)?', content, re.M|re.S)

    if re_match is None:
        return S_ERR, (None, None, None, None, None)

    main_content = re_match.group(1)
    origin = '\xa1\xb0 \xb5o\xabH\xaf\xb8:' + re_match.group(2)
    the_from = re_match.group(4)
    first_comment = ''
    the_rest_comments = ''

    return S_OK, (main_content, origin, the_from, first_comment, the_rest_comments)



def _main():
    filename = sys.argv[1]

    with open(filename, 'r') as f:
        content = f.read()

    error, (main_content, origin, the_from, first_comment, the_rest_comments) = sep_main_comments(content)

    if error:
        logging.error('unable to sep main-comments, assuming no comments')
        error, (main_content, origin, the_from, first_comment, the_rest_comments) = sep_main(content)
        if error:
            logging.error('unable to sep main')
            main_content, origin, the_from, comment, the_rest_comments = (content, '', '', '', '')

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