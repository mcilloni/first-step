include includes.mk

ifndef CC
	CC=clang
endif

ifndef AR
	AR=ar
endif

ifndef RANLIB
	RANLIB=ranlib
endif

VERSION = $(shell cat ../VERSION)

.PHONY: all clean

all: 
	$(MAKE) -C utils 
	$(MAKE) -C lex 
	$(MAKE) -C parse 
	$(MAKE) -C cgen 
	$(MAKE) -C helmc1 
	cp -f ../build/helmc1-build/helmrt.o ../build
	cp -f header.txt ../build/helmc
	cat helmc.py >> ../build/helmc
	chmod +x ../build/helmc
	cp -f header.txt ../build/helml
	cat helml.py >> ../build/helml
	chmod +x ../build/helml
	$(MAKE) -C libhelm


clean:
	$(MAKE) -C libhelm clean
	$(MAKE) -C utils clean 
	$(MAKE) -C lex clean
	$(MAKE) -C parse clean 
	$(MAKE) -C cgen clean 
	$(MAKE) -C helmc1 clean 
	rm -f ../build/helmc
	rm -f ../build/helml

