ifndef HELMC
	HELMC=helmc
endif

ifndef CC
	CC=clang
endif

ifndef AR
	AR=ar
endif

ifndef RANLIB
	RANLIB=ranlib
endif

.PHONY: all clean

all: 
	mkdir -p ../../build/libhelm/
	$(MAKE) -C libc
	$(AR) rc ../../build/libhelm.a ../../build/libhelm/*.o
	$(RANLIB) ../../build/libhelm.a
	mkdir -p ../../build/libhelm/hemd/
	cp *.hemd ../../build/libhelm/hemd/


clean:
	$(MAKE) -C libc clean 
	rm -f ../../build/libhelm.a
	rm -rf ../../build/libhelm/

