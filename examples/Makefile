include includes.mk

ifndef CC
	CC=clang
endif

ifndef RANLIB
	RANLIB=ranlib
endif

.PHONY: all clean

all: 
	$(MAKE) -C utils 
	$(MAKE) -C lex 
	$(MAKE) -C parse 
	cp -f ../build/examples/parse/parsex ../build/helmtree

clean:
	$(MAKE) -C utils clean 
	$(MAKE) -C lex clean 
	$(MAKE) -C parse clean 
	rm -rf ../build/examples/
