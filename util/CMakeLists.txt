cmake_minimum_required(VERSION 2.6.4)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPTTBBS_UTIL")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(BBSBASE "${PROJECT_SOURCE_DIR}/include/var.h")
set(UTIL_OBJS
    util_var.o
    )

set(MBBSD_OBJS
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
    bbsmail
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

foreach(fn MBBSD_OBJS)
    set(obj "${CMAKE_CURRENT_BINARY_DIR}/util_${fn}.o")
    add_custom_command(OUTPUT ${obj}
        COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -D_BBS_UTIL_C_ -o ${obj} ${PROJECT_SOURCE_DIR}/mbbsd/${fn}.c
        )    
endforeach()

foreach(prog ${CPROG_WITH_UTIL})
    add_executable(${prog}
        ${prog}.c
        )
    target_link_libraries(${prog} ${LDLIBS} ${UTIL_OBJS})
endforeach()
