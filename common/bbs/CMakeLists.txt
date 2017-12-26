cmake_minimum_required(VERSION 3.0.2)

set(CMBBS_SRCS
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

add_library(cmbbs STATIC ${CMBBS_SRCS})
add_dependencies(cmbbs VAR_H)
link_directories(${CMAKE_BINARY_DIR})
