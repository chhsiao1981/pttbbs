# $Id$

SRCROOT:= ../..
.include "$(SRCROOT)/pttbbs.mk"

PROGS = test_dummy

TEST_OBJS = test_dummy.o

@echo OBJS- $(OBJS)

.for fn in $(PROGS)
$(fn): $(OBJS) $(fn).o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(fn) $(OBJS) $(LDLIBS) $(fn).o
.endfor

.cc.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(PROGS)

clean:
	rm -f *.o $(PROGS)