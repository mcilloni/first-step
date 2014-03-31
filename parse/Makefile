ifndef CC
	CC=clang 
endif

LD=$(CC)

AR=ar

ifndef RANLIB
	RANLIB=ranlib
endif


CFLAGS=

ifndef WINDOWS
	CFLAGS += -rdynamic
endif

UNAME := $(shell uname -s)

ifeq ($(UNAME),FreeBSD)
        CFLAGS += "-lexecinfo"
endif                         

LD += $(CFLAGS)

all: clean
	$(CC) -c expr.c -g -Wall -pedantic -std=c11
	$(CC) -c nonterminals.c -g -Wall -pedantic -std=c11 
	$(CC) -c pnleaves.c -g -Wall -pedantic -std=c11
	$(CC) -c ptree.c -g -Wall -pedantic -std=c11
	$(CC) -c symbols.c -g -Wall -pedantic -std=c11 
	$(CC) -c types.c -g -Wall -pedantic -std=c11
	$(CC) -c parse.c -g -Wall -pedantic -std=c11
	$(AR) -rc libparse.a *.o
	$(RANLIB) libparse.a
	cp libparse.a ..
	$(LD) -o evalex evalex.c -L..  -lparse -ltreemap -llex -lutils -larray -llist -g -Wall -pedantic -std=gnu11
	$(LD) -o exprex exprex.c -L..  -lparse -ltreemap -llex -lutils -larray -llist -g -Wall -pedantic -std=gnu11
	$(LD) -o posex posex.c -L..  -lparse -ltreemap -llex -lutils -larray -llist -g -Wall -pedantic	 -std=gnu11
	$(LD) -o parsex parsex.c -L..  -lparse -ltreemap -llex -lutils -larray -llist -g -Wall -pedantic	 -std=gnu11
	$(LD) -o symex symex.c -L..  -lparse -ltreemap -larray -lutils -g -Wall -pedantic	 -std=gnu11
	$(LD) -o treex treex.c -L..  -lparse -llex -ltreemap -lutils -llist -larray -g -Wall -pedantic -std=gnu11	
	$(LD) -o typex typex.c -L..  -lparse -llex -ltreemap -lutils -larray -g -Wall -pedantic	 -std=gnu11
	$(LD) -o varex varex.c -L..  -lparse -ltreemap -llex -lutils -larray -llist -g -Wall -pedantic  -std=gnu11
	$(LD) -o assignex assignex.c -L..  -lparse -ltreemap -llex -lutils -larray -llist -g -Wall -pedantic  -std=gnu11

clean:
	rm -f *.{o,a,so,gch}
	rm -f *ex
