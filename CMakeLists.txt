cmake_minimum_required(VERSION 2.6.4)

project(pttbbs)

message(STATUS "CMAKE_SYSTEM: ${CMAKE_SYSTEM}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_VERSION: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")

if(${CMAKE_SYSTEM_PROCESSOR} == "x86_64")
    set(SHM_MB_SIZE 4)
    set(TIMET64)
elseif(${CMAKE_SYSTEM_PROCESSOR} == "ia64")
    set(SHM_MB_SIZE 256)
    set(TIMET64)    
endif()

configure_file(pttbbs.cmake.conf pttbbs.conf @only)

# add_subdirectory(common)
# add_subdirectory(tests)
