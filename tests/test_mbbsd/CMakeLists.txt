cmake_minimum_required(VERSION 3.0.2)

message("SRCS: ${SRCS}")

add_executable(test_dummy
    test_dummy.cc
    )

target_link_libraries(test_dummy gtest gtest_main ${LDLIBS})

add_executable(test_pttutil
    test_pttutil.cc
    ${CMAKE_SOURCE_DIR}/mbbsd/pttutil.c
    )

target_link_libraries(test_pttdb gtest gtest_main ${LDLIBS})

add_executable(test_pttdb
    test_pttdb.cc
    ${CMAKE_SOURCE_DIR}/mbbsd/pttutil.c
    ${CMAKE_SOURCE_DIR}/mbbsd/pttdb.c
    )

target_link_libraries(test_pttdb gtest gtest_main ${LDLIBS})
