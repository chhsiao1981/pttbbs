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

##########
# gtest
##########
gtest-all.o : $(GTEST_SRCS_)
        $(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
        $(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
        $(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
        $(AR) $(ARFLAGS) $@ $^

##########
# all
##########
.for fn in $(TEST_PROGS)
$(fn): $(MBBSD_OBJS) $(fn).o gtest_main.a
	@echo OBJS: $(OBJS)
	@echo MBBSD_OBJS: $(MBBSD_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -lpthread -o $(fn) $(MBBSD_OBJS)  $(bfn).o
.endfor

all: $(TEST_PROGS)

clean:
	rm -f *.o $(TEST_PROGS)

.endif