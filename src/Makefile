include includes.mk

ifndef CC
	CC=clang
endif

ifndef RANLIB
	RANLIB=ranlib
endif

all: 
	$(MAKE) -C utils DEPTH=.
	$(MAKE) -C lex DEPTH=.
	$(MAKE) -C parse DEPTH=.
	$(MAKE) -C cgen DEPTH=.
	$(MAKE) -C helmc1 DEPTH=.
	cp -f ../build/helmc1-build/helmrt.o ../build
	cp -f helmc.sh ../build/helmc
	chmod +x ../build/helmc

clean:
	$(MAKE) -C utils clean DEPTH=.
	$(MAKE) -C lex clean DEPTH=.
	$(MAKE) -C parse clean DEPTH=.
	$(MAKE) -C cgen clean DEPTH=.
	$(MAKE) -C helmc1 clean DEPTH=.
	rm -f ../build/helmc
