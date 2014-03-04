CC=clang
AR=ar
RANLIB=ranlib

all:
	$(CC) -c symbols.c -g -Wall -pedantic -std=gnu11
	$(AR) -rc libparse.a symbols.o
	$(RANLIB) libparse.a
	cp libparse.a ..
	rm *.o
	$(CC) -o symex symex.c -L.. -ljemalloc -lparse -ltreemap -lutils -g -Wall -pedantic	

