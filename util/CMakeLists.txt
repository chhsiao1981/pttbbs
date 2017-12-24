cmake_minimum_required(VERSION 2.6.4)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPTTBBS_UTIL")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(BBSBASE "${PROJECT_SOURCE_DIR}/include/var.h")
set(UTIL_SRCS
    util_var.c
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

foreach(prog CPROG_WITH_UTIL)
    add_executable(${prog}
        ${UTIL_SRCS}
        ${prog}.c
        )
    target_link_libraries(${prog} ${LDLIBS})

endforeach(CPROG_WITH_UTIL)
