# $Id$

SRCROOT:= ../..
.include "$(SRCROOT)/pttbbs.mk"

PROGS = test_dummy

TEST_OBJS = test_dummy.o

.for fn in $(PROGS)
$(fn): $(OBJS) $(fn).o
	@echo OBJS- $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(fn) $(OBJS) $(LDLIBS) $(fn).o
.endfor

.cc.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(PROGS)

clean:
	rm -f *.o $(PROGS)
