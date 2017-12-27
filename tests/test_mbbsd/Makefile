# $Id$

##########
# started from mbbsd Makefile
##########

.include "$(SRCROOT)/mbbsd/Makefile"

##########
# test
##########

SRCROOT:= ../..
BBSCONF:= $(SRCROOT)/pttbbs.conf

.for obj in $(OBJS)
MBBSD_OBJS = $(MBBSD_OBJS) $(SRCROOT)/mbbsd/$(obj)
.endfor

TEST_PROGS = test_dummy

TEST_OBJS = test_dummy.o

.for fn in $(TEST_PROGS)
$(fn): $(OBJS) $(fn).o
	@echo OBJS: $(OBJS)
	@echo MBBSD_OBJS: $(MBBSD_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(fn) $(MBBSD_OBJS) $(LDLIBS) $(fn).o
.endfor

override all: $(TEST_PROGS)

override clean:
	rm -f *.o $(PROGS)
