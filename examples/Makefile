include includes.mk

ifndef CXX
	CXX=clang++
endif

ifndef RANLIB
	RANLIB=ranlib
endif

.PHONY: all clean

all: 
	$(MAKE) -C utils 
	$(MAKE) -C lex 
	$(MAKE) -C parse 

clean:
	$(MAKE) -C utils clean 
	$(MAKE) -C lex clean 
	$(MAKE) -C parse clean 
	rm -rf ../build/examples/
