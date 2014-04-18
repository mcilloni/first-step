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
	$(MAKE) -C backtrace
	$(MAKE) -C memstream
	$(MAKE) -C array
	$(MAKE) -C list
	$(MAKE) -C treemap
	cp */*.a ../build

clean:
	$(MAKE) -C array clean
	$(MAKE) -C backtrace clean
	$(MAKE) -C memstream clean
	$(MAKE) -C list clean
	$(MAKE) -C treemap clean
	rm -f ../build/array.a
	rm -f ../build/backtrace.a
	rm -f ../build/list.a
	rm -f ../build/treemap.a


