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
	$(CC) -c lex.c -g -Wall -pedantic -std=c11
	$(AR) -rc liblex.a lex.o
	$(RANLIB) liblex.a
	cp liblex.a ..
	$(CC) -o sample sample.c -L..  -llex -lutils -g -Wall -pedantic	-std=c11

clean:
	rm -f *.{o,so,a,gch}
	rm -f sample
