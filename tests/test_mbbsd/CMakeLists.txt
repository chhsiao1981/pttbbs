cmake_minimum_required(VERSION 2.6.4)

add_executable(test_dummy
    test_dummy.cc
    )

target_link_libraries(test_dummy gtest gtest_main cmbbs cmsys osdep)

file(GLOB MBBSD_OBJS ${PROJECT_SOURCE_DIR}/mbbsd/*.o)

add_executable(test_migrate_merge
    test_migrate_merge.cc
    ${MBBSD_OBJS}
    )

target_link_libraries(test_migrate_merge gtest gtest_main cmbbs cmsys osdep)
