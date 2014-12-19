include includes.mk

ifndef CXX
	CXX=clang++
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
	$(MAKE) -C forkc1 
	cp -f header.txt ../build/forkc
	cat forkc.py >> ../build/forkc
	chmod +x ../build/forkc
	cp -f header.txt ../build/forkl
	cat forkl.py >> ../build/forkl
	chmod +x ../build/forkl
	$(MAKE) -C libfork


clean:
	$(MAKE) -C libfork clean
	$(MAKE) -C utils clean 
	$(MAKE) -C lex clean
	$(MAKE) -C parse clean 
	$(MAKE) -C cgen clean 
	$(MAKE) -C forkc1 clean 
	rm -f ../build/forkc
	rm -f ../build/forkl

