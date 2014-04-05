ifndef CC
	CC=clang
endif

AR=ar

ifndef RANLIB
	RANLIB=ranlib
endif

CFLAGS=

UNAME := $(shell uname -s)

ifeq ($(UNAME),FreeBSD)
        CFLAGS += "-lexecinfo"
endif                         

CC += $(CFLAGS)

all: clean
	$(CC) -o sample sample.c -L../../build -I../../src  -llex -lutils -g -Wall -pedantic	-std=c11

clean:
	rm -f sample
