cmake_minimum_required(VERSION 2.6.4)

set(SRC
    daemon.c
    file.c
    lock.c
    log.c
    net.c
    sort.c
    string.c
    time.c
    crypt.c
    record.c
    vector.c
    telnet.c
    vbuf.c
    vtkbd.c
    utf8.c
    big5.c
    buffer.c
    thttp.c
    )

add_custom_command(
    OUTPUT big5.c
    COMMAND big5_gen.py > big5.c
    DEPENDS big5data.tar.bz2 big5_gen.py
    )

add_library(cmsys ${SRC} big5.c)
