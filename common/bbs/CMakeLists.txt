cmake_minimum_required(VERSION 2.6.4)

set(SRC
    log.c
    money.c
    names.c
    path.c
    time.c
    string.c
    fhdr_stamp.c
    cache.c
    passwd.c
    filehdr.c
    banip.c
    )

add_library(cmbbs STATIC
    ${SRC}
    )
