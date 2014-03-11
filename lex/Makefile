CC=clang
AR=ar
RANLIB=ranlib

all:
	$(CC) -c lex.c -g -Wall -pedantic -std=c11
	$(AR) -rc liblex.a lex.o
	$(RANLIB) liblex.a
	cp liblex.a ..
	rm *.o
	$(CC) -o sample sample.c -L.. -ljemalloc -llex -lutils -g -Wall -pedantic	

