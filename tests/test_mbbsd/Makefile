# $Id$

##########
# use googletest as test-framework
##########

SRCROOT:= ../..
.include "$(SRCROOT)/pttbbs.mk"

MBBSD_DIR:=	${SRCROOT}/mbbsd

##########
# gtest
##########

CPPFLAGS+=	-isystem $(GTEST_DIR)/include
CXXFLAGS+=	-g -Wall -Wextra -pthread

GTEST_HEADERS=	/usr/include/gtest/*.h \
		/usr/include/gtest/internal/*.h

GTEST_SRCS_= $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

AR:=		ar
ARFLAGS:=	rv

#######################################################################
# mbbsd modules
#######################################################################
#COREOBJS = bbs.o announce.o read.o board.o brc.o mail.o record.o fav.o
COREOBJS =
#ABUSEOBJS = captcha.o
ABUSEOBJS =
#ACCOBJS  = user.o acl.o register.o passwd.o emaildb.o
ACCOBJS  =
#NETOBJS  = mbbsd.o io.o term.o telnet.o nios.o
NETOBJS  = nios.o
#TALKOBJS = friend.o talk.o ccw.o
TALKOBJS =
#UTILOBJS = stuff.o kaede.o convert.o name.o syspost.o cache.o cal.o
UTILOBJS = stuff.o convert.o
#UIOBJS   = menu.o vtuikit.o psb.o
UIOBJS   = vtuikit.o
#PAGEROBJS= more.o pmore.o
PAGEROBJS=
#PLUGOBJS = ordersong.o angel.o timecap.o
PLUGOBJS =
#CHESSOBJS= chess.o chc.o chc_tab.o ch_go.o ch_gomo.o ch_dark.o ch_reversi.o ch_conn6.o
CHESSOBJS=
#GAMEOBJS = chicken.o gamble.o
GAMEOBJS =

MISC_OBJS:=
.if $(USE_PFTERM)
MISC_OBJS+=		pfterm.o
.else
MISC_OBJS+=		screen.o
.endif

MBBSD_COREOJBS:=
.for obj in ${COREOBJS}
MBBSD_COREOBJS+= ${MBBSD_DIR}/${obj}
.endfor
MBBSD_ABUSEOBJS:=
.for obj in ${ABUSEOBJS}
MBBSD_ABUSEOBJS+= ${MBBSD_DIR}/${obj}
.endfor
MBBSD_NETOBJS:=
.for obj in ${NETOBJS}
MBBSD_NETOBJS+= ${MBBSD_DIR}/${obj}
.endfor
MBBSD_TALKOBJS:=
.for obj in ${TALKOBJS}
MBBSD_TALKOBJS+= ${MBBSD_DIR}/${obj}
.endfor
MBBSD_UTILOBJS:=
.for obj in ${UTILOBJS}
MBBSD_UTILOBJS+= ${MBBSD_DIR}/${obj}
.endfor
MBBSD_TALKOBJS:=
.for obj in ${PAGEROBJS}
MBBSD_PAGEROBJS+= ${MBBSD_DIR}/${obj}
.endfor
MBBSD_PLUGOBJS:=
.for obj in ${PLUGOBJS}
MBBSD_PLUGOBJS+= ${MBBSD_DIR}/${obj}
.endfor
MBBSD_CHESSOBJS:=
.for obj in ${CHESSOBJS}
MBBSD_CHESSOBJS+= ${MBBSD_DIR}/${obj}
.endfor
MBBSD_GAMEOBJS:=
.for obj in ${GAMEOBJS}
MBBSD_GAMEOBJS+= ${MBBSD_DIR}/${obj}
.endfor
MBBSD_MISCOBJS:=
.for obj in ${MISCOBJS}
MBBSD_MISCOBJS+= ${MBBSD_DIR}/${obj}
.endfor

MBBSD_OBJS:= ${MBBSD_COREOBJS} ${MBBSD_ABUSEOBJS} ${MBBSD_NETOBJS} ${MBBSD_TALKOBJS} ${MBBSD_UTILOBJS} ${MBBSD_PAGEROBJS} ${MBBSD_PLUGOBJS} ${MBBSD_CHESSOJBS} ${MBBSD_GAMEOBJS} ${MBBSD_MISCOBJS}

#######################################################################
# reduce .bss align overhead
#######################################################################

.if !defined(DEBUG) && $(OSTYPE)!="Darwin"
LDFLAGS+=-Wl,--sort-common
.endif

#######################################################################
# common libraries
#######################################################################

LDFLAGS+= -L$(SRCROOT)/common/bbs -L$(SRCROOT)/common/sys \
	  -L$(SRCROOT)/common/osdep
LDLIBS:= -lcmbbs -lcmsys -losdep $(LDLIBS)

#######################################################################
# conditional configurations and optional modules
#######################################################################

BBSCONF:=	$(SRCROOT)/pttbbs.conf
DEF_PATTERN:=	^[ \t]*\#[ \t]*define[ \t]*
DEF_CMD:=	grep -Ewq "${DEF_PATTERN}"
DEF_YES:=	&& echo "YES" || echo ""
USE_BBSLUA!=  	sh -c '${DEF_CMD}"USE_BBSLUA" ${BBSCONF} ${DEF_YES}'
USE_PFTERM!=	sh -c '${DEF_CMD}"USE_PFTERM" ${BBSCONF} ${DEF_YES}'
USE_NIOS!=	sh -c '${DEF_CMD}"USE_NIOS"   ${BBSCONF} ${DEF_YES}'
USE_CONVERT!=	sh -c '${DEF_CMD}"CONVERT"    ${BBSCONF} ${DEF_YES}'
MONGO_CLIENT_URL!=	sh -c '${DEF_CMD}"MONGO_CLIENT_URL" ${BBSCONF} ${DEF_YES}'

.if $(MONGO_CLIENT_URL)
MONGO_CXXFLAGS:=	-I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0	
MONGO_LDFLAGS:= -L/usr/local/lib -Wl,-rpath,/usr/local/lib -rdynamic
MONGO_LIBS:=	-lmongoc-1.0 -lbson-1.0 -lresolv
CXXFLAGS+=	${MONGO_CXXFLAGS}
LDFLAGS+=	${MONGO_LDFLAGS}
LDLIBS+=	${MONGO_LIBS}
MBBSD_OBJS+=	${MBBSD_DIR}/var.o ${MBBSD_DIR}/pttdb.o ${MBBSD_DIR}/pttdb_main.o ${MBBSD_DIR}/pttdb_comment.o ${MBBSD_DIR}/pttdb_comment_reply.o ${MBBSD_DIR}/pttdb_const.o ${MBBSD_DIR}/pttdb_content_block.o ${MBBSD_DIR}/pttdb_uuid.o ${MBBSD_DIR}/pttdb_util.o ${MBBSD_DIR}/util_db.o ${MBBSD_DIR}/pttdb_dict_bson_by_uu.o ${MBBSD_DIR}/migrate_db_to_file.o ${MBBSD_DIR}/migrate_file_to_db.o
.endif # MONGO_CLIENT_URL

##########
# test
##########

TEST_PROGS=	test_dummy

.if $(MONGO_CLIENT_URL)
TEST_PROGS+=	test_pttdb test_pttdb_main test_pttdb_comment test_pttdb_comment_reply test_pttdb_content_block test_pttdb_uuid test_pttdb_util test_util_db test_pttdb_dict_bson_by_uu test_migrate_db_to_file test_migrate_file_to_db
.endif

##########
# TARGETS
##########

.if defined(GTEST_DIR)
##########
# gtest
##########
gtest-all.o: $(GTEST_SRCS_)
	$(CXX) -I$(GTEST_DIR) $(CXXFLAGS) -c $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o: $(GTEST_SRCS_)
	$(CXX) -I$(GTEST_DIR) $(CXXFLAGS) -c $(GTEST_DIR)/src/gtest_main.cc

gtest.a: gtest-all.o
	$(AR) $(ARFLAGS) $@ $>

gtest_main.a: gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $>

##########
# all
##########

.SUFFIXES: .cc .o

.cc.o: $(SRCROOT)/include/var.h
	$(CXX) $(CXXFLAGS) -c $*.cc

.for fn in ${TEST_PROGS}
${fn}: ${fn}.o gtest.a ${MBBSD_OBJS}
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $> $(LDLIBS)
.endfor


all: $(TEST_PROGS)

.else # GTEST_DIR

all:

.endif # GTEST_DIR

clean:
	rm -f *.o gtest.a gtest_main.a $(TEST_PROGS)
