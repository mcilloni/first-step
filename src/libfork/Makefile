ifndef FORKC
	FORKC=../../build/forkc
endif

ifndef CXX
	CXX=clang++
endif

ifndef AR
	AR=ar
endif

ifndef RANLIB
	RANLIB=ranlib
endif

.PHONY: all clean

all: 
	mkdir -p ../../build/libfork/ford/
	cp *.ford ../../build/libfork/ford/
	$(MAKE) -C spring
	$(AR) rc ../../build/libfork.a ../../build/libfork/*.o
	$(CXX) -shared -o ../../build/libfork/libfork.so ../../build/libfork/*.o
	$(RANLIB) ../../build/libfork.a
	$(FORKC) -o ../../build/forkrt.o forkrt.fork


clean:
	$(MAKE) -C spring clean 
	rm -f ../../build/libfork.a
	rm -rf ../../build/libfork/

