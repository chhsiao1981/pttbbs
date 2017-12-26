cmake_minimum_required(VERSION 3.0.2)

set(CORESRCS
    bbs.c
    announce.c
    read.c
    board.c
    brc.c
    mail.c
    record.c
    fav.c
    )

set(ABUSESRCS
    captcha.c
    )

set(ACCSRCS
    user.c
    acl.c
    register.c
    passwd.c
    emaildb.c
    )

set(MBBSDOBJ
    mbbsd.o
    )

set(MBBSDSRC
    mbbsd.c
    )

set(NETSRCS
    io.c
    term.c
    telnet.c
    nios.c
    )

set(TALKSRCS
    friend.c
    talk.c
    ccw.c
    )

set(UTILSRCS
    stuff.c
    kaede.c
    convert.c
    name.c
    syspost.c
    cache.c
    cal.c
    )

set(UISRCS
    menu.c
    vtuikit.c
    psb.c
    )

set(PAGERSRCS
    more.c
    pmore.c
    )

set(PLUGSRCS
    ordersong.c
    angel.c
    timecap.c
    )

set(CHESSSRCS
    chess.c
    chc.c
    chc_tab.c
    ch_go.c
    ch_gomo.c
    ch_dark.c
    ch_reversi.c
    ch_conn6.c
    )

set(GAMESRCS
    chicken.c
    gamble.c
    )

set(OTHERSRCS
    admin.c
    assess.c
    edit.c
    xyz.c
    var.c
    vote.c
    voteboard.c
    comments.c
    )

set(SRCS
    ${OTHERSRCS}
    ${CORESRCS}
    ${ABUSESRCS}
    ${ACCSRCS}
    ${NETSRCS}
    ${TALKSRCS}
    ${UTILSRCS}
    ${UISRCS}
    ${PAGERSRCS}
    ${PLUGSRCS}
    ${CHESSSRCS}
    ${GAMESRCS})

set(TESTSZSRCS
    testsz.c
    )

##########
# diet
# XXX also in mbbsd.c
##########

if(${DIET})
    set(LDLIBS ${LDLIBS} cmdiet)
    set(DIETCC "diet -Os")
endif()

##########
# reduce .bss align overhead
##########

if(NOT "${DEBUG}" AND NOT ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin"))
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--sort-common")
endif()

##########
# bbslua
##########

if(${USE_BBSLUA})
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
        set(LUA_PKG_NAME "lua-5.1")
        set(LDLIBS "${LDLIBS} -Wl,--no-as-needed")
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(LUA_PKG_NAME "lua")
    else()
        set(LUA_PKG_NAME "lua5.1")
    endif()
    execute_process(COMMAND pkg-config --cflags "${LUA_PKG_NAME}" 
        OUTPUT_VARIABLE LUA_CFLAGS
        )
    execute_process(COMMAND pkg-config --libs "${LUA_PKG_NAME}"
        OUTPUT_VARIABLE LUA_LIBS
        )
    string(STRIP "${LUA_CFLAGS}" LUA_CFLAGS)
    string(STRIP "${LUA_LIBS}" LUA_LIBS)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LUA_CFLAGS}")
    set(LDLIBS "${LDLIBS} ${LUA_LIBS}")
    set(SRCS ${SRCS} bbslua.c bbsluaext.c)
endif()

##########
# pfterm
##########

if(${USE_PFTERM})
    set(SRCS ${SRCS} pfterm.c)
else()
    set(SRCS ${SRCS} screen.c)
endif()

##########
# testsz
##########

add_executable(testsz 
    ${TESTSZSRCS}
    )

##########
# vers.c
##########

add_custom_command(OUTPUT vers.c
    COMMAND sh ${PROJECT_SOURCE_DIR}/util/newvers.sh
    DEPENDS ${PROJECT_SOURCE_DIR}/util/newvers.sh
    )

##########
# mbbsd.o
##########

# XXX hack for FLAGS
string(REPLACE " " ";" FLAGS ${CMAKE_C_FLAGS})
if(("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin") AND NOT ("${CMAKE_OSX_SYSROOT}" STREQUAL ""))
    set(FLAGS ${FLAGS} -isysroot ${CMAKE_OSX_SYSROOT})
endif()

# XXX hack for DIETCC
string(REPLACE " " ";" DIETCC "${DIETCC}")

# XXX hack for rule_launch_compile
get_property(RULE_LAUNCH_COMPILE GLOBAL PROPERTY RULE_LAUNCH_COMPILE)

message(STATUS "FLAGS for mbbsd.o: ${FLAGS} DIETCC: ${DIETCC} RULE_LAUNCH_COMPILE: ${RULE_LAUNCH_COMPILE}")

add_custom_command(OUTPUT ${MBBSDOBJ}
    COMMAND ${DIETCC} ${RULE_LAUNCH_COMPILE} ${CMAKE_C_COMPILER} ${FLAGS} -o ${MBBSDOBJ} -c ${CMAKE_CURRENT_SOURCE_DIR}/${MBBSDSRC}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${MBBSDSRC}
    )

set_source_files_properties(${MBBSDOBJ}
    PROPERTIES
    EXTERNAL_OBJECT TRUE
    GENERATED TRUE
    )

# DIET
if(${DIET})
    add_executable(mbbsd vers.c ${SRCS} ${MBBSDOBJ})
else()
    add_executable(mbbsd vers.c ${SRCS} ${MBBSDSRC})
endif()
target_link_libraries(mbbsd ${LDLIBS})
