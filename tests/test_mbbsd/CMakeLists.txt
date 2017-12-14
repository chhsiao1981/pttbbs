cmake_minimum_required(VERSION 2.6.4)

add_executable(test_dummy
    test_dummy.cc
    )

target_link_libraries(test_dummy gtest gtest_main)

add_executable(test_migrate_merge
    test_migrate_merge.cc
    ${PROJECT_SOURCE_DIR}/mbbsd/migrate_merge.o
    ${PROJECT_SOURCE_DIR}/mbbsd/post3.o
    ${PROJECT_SOURCE_DIR}/mbbsd/comments.o
    )

target_link_libraries(test_migrate_merge gtest gtest_main cmbbs cmsys osdep)
