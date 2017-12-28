# $Id$

SRCROOT=	..
.include "$(SRCROOT)/pttbbs.mk"
.include "mbbsdvars.mk"

#######################################################################
# Make Rules
#######################################################################

.SUFFIXES: .c .o
.PHONY: all ctags test install clean

.c.o:	$(SRCROOT)/include/var.h
	$(CC) $(CFLAGS) -c $*.c

all: $(PROG)

$(PROG): testsz $(OBJS)
	sh $(SRCROOT)/util/newvers.sh
	@printf "\033[0;1;32m"
	@[ -z "$(DEBUG)" ] || printf "\033[31m+DEBUG \033[32m"
	@[ -z "$(USE_BBSLUA)" ] || echo -n "+BBSLUA "
	@[ -z "$(USE_PFTERM)" ] || echo -n "+PFTERM "
	@[ -z "$(USE_CONVERT)" ] || echo -n "+CONVERT "
	@[ -z "$(USE_NIOS)" ] || echo -n "+NIOS "
	@printf "\033[m"
	$(CC) vers.c -o $(PROG) $(OBJS) $(LDFLAGS) $(LDLIBS)

testsz:	$(SRCROOT)/pttbbs.conf $(SRCROOT)/include/*.h testsz.c
	$(CC) $(CFLAGS) testsz.c -o testsz
	@printf "\033[0;1;33mChecking configuration data sizes...\033[m\n"
	@./testsz
	@printf "\033[0;1;32mData size configuration OK\033[m\n"

mbbsd.o: mbbsd.c $(SRCROOT)/include/var.h
	$(DIETCC) $(CC) $(CFLAGS) -c $<

ctags:
	ctags *.c $(SRCROOT)/include/*.h $(SRCROOT)/common/sys/*.c $(SRCROOT)/common/bbs/*.c

# test: $(PROG)
#	killall -TERM testmbbsd || true
#	sleep 1
#	killall -9 testmbbsd || true
#	cp mbbsd testmbbsd
#	./testmbbsd -d -p9000
#	rm -f testmbbsd

install: $(PROG)
	install -d $(BBSHOME)/bin/
	install -c -m 755 $(PROG) $(BBSHOME)/bin/
	mv -f $(BBSHOME)/bin/mbbsd $(BBSHOME)/bin/mbbsd.`date '+%m%d%H%M'`
	ln -sv $(BBSHOME)/bin/mbbsd.`date '+%m%d%H%M'` $(BBSHOME)/bin/mbbsd

clean:
	rm -f $(OBJS) $(PROG)
