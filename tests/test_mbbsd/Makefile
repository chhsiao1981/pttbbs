# $Id$

##########
# started from mbbsd Makefile
##########

SRCROOT:= ../..
.include "$(SRCROOT)/mbbsd/Makefile"

##########
# test
##########

SRCROOT:= ../..
BBSCONF:= $(SRCROOT)/pttbbs.conf

# XXX hack for mbbsd flags
USE_BBSLUA!=  	sh -c '${DEF_CMD}"USE_BBSLUA" ${BBSCONF} ${DEF_YES}'
USE_PFTERM!=	sh -c '${DEF_CMD}"USE_PFTERM" ${BBSCONF} ${DEF_YES}'
USE_NIOS!=	sh -c '${DEF_CMD}"USE_NIOS"   ${BBSCONF} ${DEF_YES}'
USE_CONVERT!=	sh -c '${DEF_CMD}"CONVERT"    ${BBSCONF} ${DEF_YES}'

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

all: myall

myall: $(TEST_PROGS)

clean: myclean

myclean:
	rm -f *.o $(TEST_PROGS)
