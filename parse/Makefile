ifndef CC
	CC=clang 
endif

AR=ar

ifndef RANLIB
	RANLIB=ranlib
endif


CFLAGS=-O0 

ifndef WINDOWS
	CFLAGS += -rdynamic
endif

UNAME := $(shell uname -s)

ifeq ($(UNAME),FreeBSD)
        CFLAGS += "-lexecinfo"
endif                         

CC += $(CFLAGS)

all: clean
	$(CC) -c symbols.c -g -Wall -pedantic -std=c11 
	$(CC) -c pnleaves.c -g -Wall -pedantic -std=c11
	$(CC) -c ptree.c -g -Wall -pedantic -std=c11
	$(CC) -c parse.c -g -Wall -pedantic -std=c11
	$(CC) -c expr.c -g -Wall -pedantic -std=c11
	$(CC) -c types.c -g -Wall -pedantic -std=c11
	$(AR) -rc libparse.a *.o
	$(RANLIB) libparse.a
	cp libparse.a ..
	$(CC) -o evalex$(EXEPREFIX) evalex.c -L..  -lparse -ltreemap -llex -lutils -larray -llist -g -Wall -pedantic 
	$(CC) -o exprex$(EXEPREFIX) exprex.c -L..  -lparse -ltreemap -llex -lutils -larray -llist -g -Wall -pedantic
	$(CC) -o posex$(EXEPREFIX) posex.c -L..  -lparse -ltreemap -llex -lutils -larray -llist -g -Wall -pedantic	
	$(CC) -o symex$(EXEPREFIX) symex.c -L..  -lparse -ltreemap -lutils -g -Wall -pedantic	
	$(CC) -o treex$(EXEPREFIX) treex.c -L..  -lparse -ltreemap -lutils -larray -g -Wall -pedantic	
	$(CC) -o typex$(EXEPREFIX) typex.c -L..  -lparse -ltreemap -lutils -larray -g -Wall -pedantic	
	$(CC) -o varex$(EXEPREFIX) varex.c -L..  -lparse -ltreemap -llex -lutils -larray -llist -g -Wall -pedantic 
	$(CC) -o assignex$(EXEPREFIX) assignex.c -L..  -lparse -ltreemap -llex -lutils -larray -llist -g -Wall -pedantic 

clean:
	rm -f *.{o,a,so,gch}
	rm -f *ex$(EXEPREFIX)
