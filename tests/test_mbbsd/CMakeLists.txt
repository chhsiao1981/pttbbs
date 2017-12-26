cmake_minimum_required(VERSION 3.0.2)

add_executable(test_dummy
    test_dummy.cc
    )

target_link_libraries(test_dummy gtest gtest_main ${LDLIBS})
