# $Id$

.if defined(GTEST_DIR)
##########
# use googletest as test-framework
##########

SRCROOT:= ../..
.include "$(SRCROOT)/pttbbs.mk"

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
TEST_PROGS= test_dummy test_pttdb test_pttutil

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

test_dummy: test_dummy.o gtest_main.a $(LDLIBS)
	echo ">: $> <: $< *: $* @: $@" && $(CXX) $(CXXFLAGS) $(LDFALGS) -o $@ $>

all: $(TEST_PROGS)

.else # GTEST_DIR

all:

.endif # GTEST_DIR

clean:
	rm -f *.o gtest.a gtest_main.a $(TEST_PROGS)
