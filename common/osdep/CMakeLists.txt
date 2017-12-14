cmake_minimum_required(VERSION 2.6.4)

set(SRC
    cpuload.c
    proctitle.c
    strlcat.c
    strlcpy.c
    memusage.c
    )

add_library(osdep STATIC
    ${SRC}
    )
