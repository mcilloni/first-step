CC=clang
AR=ar
RANLIB=ranlib

all: clean
	$(CC) -c symbols.c pnleaves.c -g -Wall -pedantic -std=gnu11
	$(AR) -rc libparse.a symbols.o pnleaves.o
	$(RANLIB) libparse.a
	cp libparse.a ..
	rm *.o
	$(CC) -o symex symex.c -L.. -ljemalloc -lparse -ltreemap -lutils -g -Wall -pedantic	

clean:
	rm -f *.{o,a,so,gch}
