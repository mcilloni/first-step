CC=clang
AR=ar
RANLIB=ranlib

all:
	$(CC) -c lines.c utils.c -g -Wall -pedantic -std=gnu11
	$(AR) -rc libutils.a lines.o utils.o
	$(RANLIB) libutils.a
	cp libutils.a ..
	rm *.o
	$(CC) -o linex linex.c -L.. -ljemalloc -lutils -g -Wall -pedantic	

