CC=clang
AR=ar
RANLIB=ranlib

all: clean
	$(CC) -c lines.c utils.c env.c -g -Wall -pedantic -std=c11
	$(AR) -rc libutils.a lines.o utils.o env.o
	$(RANLIB) libutils.a
	cp libutils.a ..
	$(CC) -o linex linex.c -L.. -ljemalloc -lutils -g -Wall -pedantic	

clean:
	rm -f *.{o,gch,so,a}
	rm -f linex
