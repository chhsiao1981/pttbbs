cmake_minimum_required(VERSION 2.6.4)

project(pttbbs C)

message(STATUS "CMAKE_SYSTEM: ${CMAKE_SYSTEM}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_VERSION: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")

set(BBSHOME "${HOME}")
if("${BBSHOME}" STREQUAL "")
    set(BBSHOME /home/bbs)
endif()

# Command build flags
set(PTT_WARN "-W -Wall -Wunused -Wno-missing-field-initializers")
set(PTT_CFLAGS "${PTT_WARN} -pipe -DBBSHOME='\"${BBSHOME}\"' -I${CMAKE_SOURCE_DIR}/include")
set(PTT_CXXFLAGS "${PTT_WARN} -pipe -DBBSHOME='\"${BBSHOME}\"' -I${CMAKE_SOURCE_DIR}/include")
set(PTT_LDFLAGS "-Wl,--as-needed")

if(${CLANG})
    message(STATUS "using clang")
    set(PTT_CFLAGS "${PTT_CFLAGS} -Qunused-arguments -Wno-parentheses-equality -fcolor-diagnostics -Wno-invalid-source-encoding")
endif()

# Platform specific build flags
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    find_package(curses REQUIRED)

    set(PTT_LDFLAGS "-Wl")

    set(PTT_CFLAGS "${PTT_CFLAGS} -I/opt/local/include -I/usr/local/opt/ncurses/include -DNEED_SETPROCTITLE")
    set(PTT_CXXFLAGS "${PTT_CXXFLAGS} -I/opt/local/include -I/usr/local/opt/ncurses/include")
    set(PTT_LDFLAGS "${PTT_LDFLAGS} -L/usr/local/opt/ncurses/lib")
    set(PTT_LDLIBS "iconv ${CURSES_LIBRARIES}")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    set(PTT_CFLAGS "${PTT_CFLAGS} -I/usr/local/include")
    set(PTT_CXXFLAGS "${PTT_CXXFLAGS} -I/usr/local/include")
    set(PTT_LDFLAGS "${PTT_LDFLAGS} -L/usr/local/lib")
    set(PTT_LDLIBS "kvm iconv")
endif()

# Profiling
if(${PROFILING})
    set(PTT_CFLAGS "${PTT_CFLAGS} -pg")
    set(PTT_CXXFLAGS "${PTT_CXXFLAGS} -pg")
    set(PTT_LDFLAGS "${PTT_LDFLAGS} -pg")
    set(NO_OMITFP yes)
    set(NO_FORK yes)
endif()

# Debug
if(${DEBUG})
    set(GDB 1)
    set(PTT_CFLAGS "${PTT_CFLAGS} -DDEBUG")
    set(PTT_CXXFLAGS "${PTT_CXXFLAGS} -DDEBUG")
endif()

# flags
if(${GDB})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 ${PTT_CFLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 ${PTT_CXXFLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O0 ${PTT_LDFLAGS}")
    # set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -O0 ${PTT_LDFLAGS}")
    set(LDLIBS "${LDLIBS} ${PTT_LDLIBS}")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Os ${PTT_CFLAGS} ${EXT_CFLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Os ${PTT_CXXFLAGS} ${EXT_CXXFLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O0 ${PTT_LDFLAGS}")
    # set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -O0 ${PTT_LDFLAGS}")
    set(LDLIBS "${LDLIBS} ${PTT_LDLIBS}")

    if(${OMITFP})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fomit-frame-pointer")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer")
    endif()
endif()

message("CMAKE_C_FLAGS: ${CMAKE_C_FLAGS} CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

# No-fork
if(${NO_FORK})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_FORK")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNO_FORK")
endif()

# libevent
set(LIBEVET_CFLAGS "pkg-config --cflags libevent")
set(LIBEVENT_LIBS_L "pkg-config --libs-only-L libevent")
set(LIBEVENT_LIBS_l "pkg-config --libs-only-l libevent")

##########
# for pttbbs.conf
##########

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    message(STATUS "CMAKE SYSTEM PROCESSOR is x86_64")
    set(SHMALIGNEDSIZE 1)
    set(SHM_MB_SIZE 4)

    set(TIMET64 1)
elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "ia64")
    message("CMAKE SYSTEM PROCESSOR is ia64")
    set(SHMALIGNEDSIZE 1)
    set(SHM_MB_SIZE 256)

    set(TIMET64 1)
endif()

message(STATUS "SHM_MB_SIZE: ${SHM_MB_SIZE}")
message("LDFLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

configure_file(pttbbs.cmake.conf ${CMAKE_SOURCE_DIR}/pttbbs.conf)

##########
# var.h
##########

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/include/var.h
    COMMAND perl ${PROJECT_SOURCE_DIR}/util/parsevar.pl < ${CMAKE_SOURCE_DIR}/mbbsd/var.c > ${CMAKE_SOURCE_DIR}/include/var.h
    DEPENDS ${PROJECT_SOURCE_DIR}/mbbsd/var.c
    )

add_custom_target(VAR_H ALL
    DEPENDS ${PROJECT_SOURCE_DIR}/include/var.h)

# LIBS
set(LDLIBS "${LDLIBS} cmsys cmbbs osdep")
string(STRIP "${LDLIBS}" LDLIBS)

##########
# sub-dirs
##########

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(common)
add_subdirectory(mbbsd)
add_subdirectory(util)
# add_subdirectory(tests)
