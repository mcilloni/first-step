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
	rm -rf build
	$(MAKE) -C tests clean
