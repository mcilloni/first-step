ifndef CC
	CC=clang
endif 

AR=ar

ifndef RANLIB
	RANLIB=ranlib
endif

CFLAGS=

UNAME := $(shell uname -s)

ifeq ($(UNAME),FreeBSD)
	CFLAGS += "-lexecinfo"
endif

CC += $(CFLAGS)

all: clean
	$(CC) -c cgen.c -g -Wall -pedantic -std=c11
	$(AR) -rc libcgen.a cgen.o
	$(RANLIB) libcgen.a
	cp libcgen.a ..

clean:
	rm -f *.{o,gch,so,a}
