CC=clang
AR=ar
RANLIB=ranlib

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
	$(CC) -o posex posex.c -L..  -lparse -ltreemap -llex -lutils -larray -llist -g -Wall -pedantic	
	$(CC) -o symex symex.c -L..  -lparse -ltreemap -lutils -g -Wall -pedantic	
	$(CC) -o treex treex.c -L..  -lparse -ltreemap -lutils -larray -g -Wall -pedantic	
	$(CC) -o typex typex.c -L..  -lparse -ltreemap -lutils -larray -g -Wall -pedantic	

clean:
	rm -f *.{o,a,so,gch}
	rm -f *ex
