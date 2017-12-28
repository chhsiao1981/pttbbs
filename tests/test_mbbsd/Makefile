# $Id$

.if defined(GTEST_DIR)
##########
# use googletest as test-framework
##########

SRCROOT:= ../..
.include "$(SRCROOT)/pttbbs.mk"

##########
# include mbbsd Makefile
##########

.include "$(SRCROOT)/mbbsd/mbbsdvars.mk"

##########
# gtest
##########

CPPFLAGS+= -isystem $(GTEST_DIR)/include
CXXFLAGS+= -g -Wall -Wextra -pthread

GTEST_HEADERS= /usr/include/gtest/*.h \
                /usr/include/gtest/internal/*.h

GTEST_SRCS_= $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)                

AR:=          ar
ARFLAGS:=     rv

##########
# test
##########

.for obj in $(OBJS)
.if $(obj) != "mbbsd.o"
MBBSD_OBJS+= $(SRCROOT)/mbbsd/$(obj)
.endif
.endfor
MBBSD_VERSC= $(SRCROOT)/mbbsd/vers.c

TEST_PROGS= test_dummy

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
.for fn in $(TEST_PROGS)
$(fn): $(MBBSD_OBJS) $(fn).o gtest_main.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(fn) ${MBBSD_VERSC} $(MBBSD_OBJS) $(fn).o gtest_main.a $(LDLIBS) -lpthread 
.endfor

all: $(TEST_PROGS)

.else

all:

.endif

clean:
	rm -f *.o gtest.a gtest_main.a $(TEST_PROGS)
