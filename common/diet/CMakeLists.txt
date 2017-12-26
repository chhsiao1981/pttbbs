cmake_minimum_required(VERSION 3.0.2)

set(DIET_SRCS
    alloc.c
    random.c
    time.c
    )

add_library(cmdiet STATIC ${DIET_SRCS})
link_directories(${CMAKE_BINARY_DIR})
