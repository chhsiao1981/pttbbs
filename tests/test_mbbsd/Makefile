# $Id$

SRCROOT:= ../..
.include "$(SRCROOT)/pttbbs.mk"
.include "$(SRCROOT)/mbbsd/Makefile"

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

all: $(TEST_PROGS)

clean:
	rm -f *.o $(PROGS)
