SRCROOT:=	../..
include $(SRCROOT)/pttbbs.mk

SRCS:=  var.c
LIB:=	libvar.a
OBJS:=	$(SRCS:.c=.o)

.PHONY: all clean install

all:	$(SRCROOT)/include/var.h $(LIB)

$(LIB): $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install:

clean:
	rm -f $(OBJS) $(LIB)
