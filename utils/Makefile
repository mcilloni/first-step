CC=clang
AR=ar
RANLIB=ranlib
CFLAGS=

UNAME := $(shell uname -s)

ifeq ($(UNAME_S),FreeBSD)
	CFLAGS += "-lexecinfo"
endif

CC += $(CFLAGS)

all: clean
	$(CC) -c lines.c utils.c env.c -g -Wall -pedantic -std=c11
	$(AR) -rc libutils.a lines.o utils.o env.o
	$(RANLIB) libutils.a
	cp libutils.a ..
	$(CC) -o linex linex.c -L..  -lutils -g -Wall -pedantic	

clean:
	rm -f *.{o,gch,so,a}
	rm -f linex
