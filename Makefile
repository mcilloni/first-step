ifndef CXX
	CXX=clang++
endif

ifndef AR
	AR=ar
endif

ifndef RANLIB
	RANLIB=ranlib
endif

.PHONY: all clean ex

all: 
	$(MAKE) -C deps
	$(MAKE) -C src

ex: all
	$(MAKE) -C examples

clean:
	$(MAKE) -C deps clean 
	$(MAKE) -C src clean
	$(MAKE) -C examples clean
	rm -rf build
