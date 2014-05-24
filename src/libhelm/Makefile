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
	mkdir -p ../../build/libhelm/hemd/
	cp *.hemd ../../build/libhelm/hemd/
	$(MAKE) -C spring
	$(AR) rc ../../build/libhelm.a ../../build/libhelm/*.o
	$(CC) -shared -o ../../build/libhelm/libhelm.so ../../build/libhelm/*.o
	$(RANLIB) ../../build/libhelm.a
	$(HELMC) -o ../../build/helmrt.o helmrt.helm


clean:
	$(MAKE) -C spring clean 
	rm -f ../../build/libhelm.a
	rm -rf ../../build/libhelm/

