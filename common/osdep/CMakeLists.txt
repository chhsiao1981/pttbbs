cmake_minimum_required(VERSION 3.0.2)

set(OSDEP_SRCS
    cpuload.c
    proctitle.c
    strlcat.c
    strlcpy.c
    memusage.c
    )

add_library(osdep STATIC ${OSDEP_SRCS})
