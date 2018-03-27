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
#
# FROM mbbsd/Makefile
#
#######################################################################

COREOBJS = bbs.o announce.o read.o board.o brc.o mail.o record.o fav.o
ABUSEOBJS = captcha.o
ACCOBJS  = user.o acl.o register.o passwd.o emaildb.o
#NETOBJS  = mbbsd.o io.o term.o telnet.o nios.o
NETOBJS  = io.o term.o telnet.o nios.o
TALKOBJS = friend.o talk.o ccw.o
UTILOBJS = stuff.o kaede.o convert.o name.o syspost.o cache.o cal.o
UIOBJS   = menu.o vtuikit.o psb.o
PAGEROBJS= more.o pmore.o
PLUGOBJS = ordersong.o angel.o timecap.o
CHESSOBJS= chess.o chc.o chc_tab.o ch_go.o ch_gomo.o ch_dark.o ch_reversi.o ch_conn6.o
GAMEOBJS = chicken.o gamble.o
OBJS:=	admin.o assess.o edit.o xyz.o var.o vote.o voteboard.o comments.o \
	$(COREOBJS) $(ABUSEOBJS) $(ACCOBJS) $(NETOBJS) $(TALKOBJS) $(UTILOBJS) \
	$(UIOBJS) $(PAGEROBJS) $(PLUGOBJS) \
	$(CHESSOBJS) $(GAMEOBJS)

#######################################################################
# special library (DIET) configuration
#######################################################################

.if defined(DIET)
LDFLAGS+=	-L$(SRCROOT)/common/diet
LDLIBS+=	-ldiet
DIETCC:=	diet -Os
.endif

# reduce .bss align overhead
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

.if $(USE_BBSLUA)
.if $(OSTYPE)=="FreeBSD"
LUA_PKG_NAME?=	lua-5.1
LDLIBS+=	-Wl,--no-as-needed
.else
LUA_PKG_NAME?=	lua5.1
.endif
LUA_CFLAGS!=	pkg-config --cflags ${LUA_PKG_NAME}
LUA_LIBS!=	pkg-config --libs ${LUA_PKG_NAME}
CFLAGS+=	${LUA_CFLAGS}
LDLIBS+=	${LUA_LIBS}
OBJS+=		bbslua.o bbsluaext.o
.endif

.if $(USE_PFTERM)
OBJS+=		pfterm.o
.else
OBJS+=		screen.o
.endif

.if $(MONGO_CLIENT_URL)
MONGO_CFLAGS:=	-I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0
MONGO_LDFLAGS:=	-L/usr/local/lib
MONGO_LIBS:=	-L/usr/local/lib -lmongoc-1.0 -lbson-1.0 -lresolv
CXXFLAGS+=	${MONGO_CFLAGS}
CFLAGS+=	${MONGO_CFLAGS}
LDFLAGS+=	${MONGO_LDFLAGS}
LDLIBS+=	${MONGO_LIBS}
OBJS+=		pttdb.o pttdb_main.o pttdb_comment.o pttdb_comment_reply.o pttdb_const.o pttdb_content_block.o pttdb_uuid.o pttdb_util.o util_db.o pttdb_dict_bson_by_uu.o migrate_db_to_file.o migrate_file_to_db.o util_timestamp.o
.endif # MONGO_CLIENT_URL

#######################################################################
# mbbsd-objs
#######################################################################

MBBSD_OBJS	= mbbsd_mock.o vers_mock.o
.for obj in ${OBJS}
MBBSD_OBJS+= ${MBBSD_DIR}/${obj}
.endfor

##########
# test
##########

TEST_PROGS=	test_dummy

.if $(MONGO_CLIENT_URL)
TEST_PROGS+=	test_pttdb test_pttdb_main test_pttdb_comment test_pttdb_comment_reply test_pttdb_content_block test_pttdb_uuid test_pttdb_util test_util_db test_pttdb_dict_bson_by_uu test_migrate_db_to_file test_migrate_file_to_db test_util_timestamp
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

.SUFFIXES: .c .o

vers_mock.o: ${MBBSD_DIR}/vers.c
	$(CC) $(CFLAGS) -c $> -o $@

.c.o:	$(SRCROOT)/include/var.h
	$(CC) $(CFLAGS) -c $*.c

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
