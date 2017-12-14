cmake_minimum_required(VERSION 2.6.4)

project(pttbbs)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(common)
add_subdirectory(tests)
