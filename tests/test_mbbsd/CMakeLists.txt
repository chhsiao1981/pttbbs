cmake_minimum_required(VERSION 3.0.2)

# XXX hack for mbbsd sources
message("SRCS: ${SRCS}")
foreach(src ${SRCS})
    string(REPLACE ".c" ".o" obj ${src})
    set(test_obj ${CMAKE_BINARY_DIR}/mbbsd/${obj})
    list(APPEND MBBSD_OBJS test_obj)
endforeach()

set_source_files_properties(${MBBSD_OBJS}
    PROPERTIES
    EXTERNAL_OBJECT TRUE
    GENERATED TRUE
    )

add_executable(test_dummy
    test_dummy.cc
    )

target_link_libraries(test_dummy gtest gtest_main ${LDLIBS})

add_executable(test_pttdb
    test_pttdb.cc
    ${CMAKE_SOURCE_DIR}/mbbsd/pttdb.c
    )

target_link_libraries(test_pttdb gtest gtest_main ${LDLIBS})
