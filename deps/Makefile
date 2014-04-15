ifndef CC
	CC=clang
endif

ifndef AR
	AR=ar
endif

ifndef RANLIB
	RANLIB=ranlib
endif

UNAME = $(shell uname) 


.PHONY: all clean btrace

all: 
	install -d ../build/
ifeq ($(UNAME), OpenBSD)
	mkdir backtrace
	cp btrace-obsd/libbacktrace/backtrace.c backtrace
	cp btrace-obsd/libbacktrace/backtrace.h backtrace
	$(CC) -c -o backtrace/backtrace.o backtrace/backtrace.c
	$(AR) rc ../build/libbacktrace.a backtrace/backtrace.o
	$(RANLIB) ../build/libbacktrace.a
endif
	$(MAKE) -C array
	$(MAKE) -C list
	$(MAKE) -C treemap
	cp */*.a ../build

clean:
	$(MAKE) -C array clean
	$(MAKE) -C list clean
	$(MAKE) -C treemap clean
	rm -f ../build/array.a
	rm -f ../build/list.a
	rm -f ../build/treemap.a
	rm -rf backtrace

btrace:

