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
	$(CC) -c lex.c -g -Wall -pedantic -std=c11
	$(AR) -rc liblex.a lex.o
	$(RANLIB) liblex.a
	cp liblex.a ..
	$(CC) -o sample sample.c -L..  -llex -lutils -g -Wall -pedantic	

clean:
	rm -f *.{o,so,a,gch}
	rm -f sample
