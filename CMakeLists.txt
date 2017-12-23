cmake_minimum_required(VERSION 2.6.4)

project(pttbbs)

configure_file(pttbbs.conf pttbbs.h)
add_subdirectory(common)
add_subdirectory(tests)
