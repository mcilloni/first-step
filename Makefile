ifndef CC
	CC=clang
endif

ifndef RANLIB
	RANLIB=ranlib
endif

ifdef WINDOWS
	EXEPREFIX=.exe
else
	EXEPREFIX=
endif

UNAME := $(shell uname -s)

ifeq ($(UNAME),FreeBSD)
	MAKE=gmake
else
	MAKE=make
endif

all: clean
	$(MAKE) -C array
	$(MAKE) -C list
	$(MAKE) -C treemap
	$(MAKE) -C utils
	cp */*.a .
	$(MAKE) -C lex
	$(MAKE) -C parse	
	cp */*.a .

clean:
	$(MAKE) -C array clean
	$(MAKE) -C list clean
	$(MAKE) -C treemap clean
	$(MAKE) -C utils clean
	$(MAKE) -C lex clean
	$(MAKE) -C parse clean
	rm -f *.a