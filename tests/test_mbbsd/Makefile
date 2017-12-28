# $Id$

##########
# use googletest as test-framework
##########

.if .defined(GTEST_DIR)

SRCROOT:= ../..
.include "$(SRCROOT)/pttbbs.mk"

##########
# include mbbsd Makefile
##########

.include "$(SRCROOT)/mbbsd/mbbsdvars.mk"

##########
# gtest
##########

CPPFLAGS += -isystem $(GTEST_DIR)/include
CXXFLAGS += -g -Wall -Wextra -pthread

GTEST_HEADERS = /usr/include/gtest/*.h \
                /usr/include/gtest/internal/*.h

GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)                

##########
# test
##########

.for obj in $(OBJS)
MBBSD_OBJS += $(SRCROOT)/mbbsd/$(obj)
.endfor

TEST_PROGS = test_dummy

.for fn in $(TEST_PROGS)
$(fn): $(MBBSD_OBJS) $(fn).o
	@echo OBJS: $(OBJS)
	@echo MBBSD_OBJS: $(MBBSD_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -lpthread -o $(fn) $(MBBSD_OBJS)  $(bfn).o
.endfor

.SUFFIXES: .cc .o
.cc.o:	$(SRCROOT)/include/var.h $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

all: $(TEST_PROGS)

clean:
	rm -f *.o $(TEST_PROGS)

.endif