ifndef CC
	CC=clang
endif 

CFLAGS=

UNAME := $(shell uname -s)

ifeq ($(UNAME),FreeBSD)
	CFLAGS += "-lexecinfo"
endif

CC += $(CFLAGS)

all: clean
	$(CC) -c helmrt.c -g -Wall -pedantic -std=c11
	$(CC) -rdynamic -o firststep firststep.c -L.. -lcgen -lparse -llex -lutils -llist -ltreemap -larray -g -Wall -pedantic -std=gnu11
clean:
	rm -f *.o
	rm -f firststep
