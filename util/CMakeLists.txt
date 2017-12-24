cmake_minimum_required(VERSION 2.6.4)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPTTBBS_UTIL")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(BBSBASE "${PROJECT_SOURCE_DIR}/include/var.h")
set(UTIL_OBJS
    util_var.o
    )

set(MBBSD_SRCS
    var
    )

set(CPROG_WITH_UTIL
    boardlist
    post
    poststat
    account
    deluserfile
    expire
    mandex
    broadcast
    openticket
    topusr
    toplazyBM
    writemoney
    reaper
    buildAnnounce
    mailangel
    outmail
    chkhbf
    angel
    gamblegive
    chesscountry
    tunepasswd
    buildir
    xchatd
    uhash_loader
    timecap_buildref
    showuser
    removebm
    redir
    permreport
    setrole
    update_online
    munin
    useractive_munin
    shmctl
    bbsmail
    )

set(CPP_WITH_UTIL
    mergedir2
    buildir2
    )

set(CPROG_WITHOUT_UTIL
    showboard
    bbsrf
    initbbs
    userlist
    merge_board
    bbsctl
    )

set(PROGS
    ${CPROG_WITH_UTIL}
    ${CPROG_WITHOUT_UTIL}
    ${CPP_WITH_UTIL}
    shmctl
    backpasswd.sh
    mailog.sh
    openticket.sh
    topsong.sh
    weather.sh
    weather.perl
    toplazyBM.sh
    dailybackup.pl
    tarqueue.pl
    waterball.pl
    filtermail.pl
    getbackup.pl
    rebuildaloha.pl
    timecap_expire.sh
    )

foreach(src MBBSD_SRCS)
    set(obj ${CMAKE_CURRENT_BINARY_DIR}/util_${src}.o)
    add_custom_command(OUTPUT ${obj}
        COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -D_BBS_UTIL_C_ -c -o ${obj} ${PROJECT_SOURCE_DIR}/mbbsd/${src}.c
        DEPENDS ${PROJECT_SOURCE_DIR}/mbbsd/${src}.c
        )
endforeach()

foreach(prog ${CPROG_WITH_UTIL})
    add_executable(${prog}
        ${prog}.c
        ${UTIL_OBJS}
        )
    target_link_libraries(${prog} ${LDLIBS})
endforeach()

foreach(prog ${CPP_WITH_UTIL})
    add_executable(${prog}
        ${prog}.cc
        ${UTIL_OBJS}
        )
    target_link_libraries(${prog} ${LDLIBS})
endforeach()

foreach(prog ${CPROGR_WITHOUT_UTIL})
    add_executable(${prog}
        ${prog}.c
        )
endforeach()