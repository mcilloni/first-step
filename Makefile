ifndef CC
	CC=clang
endif

ifndef RANLIB
	RANLIB=ranlib
endif

all: 
	$(MAKE) -C deps
	$(MAKE) -C src
	$(MAKE) -C examples

clean:
	$(MAKE) -C deps clean 
	$(MAKE) -C src clean
	$(MAKE) -C examples clean
	rm -rf build
