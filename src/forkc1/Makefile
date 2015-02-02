ifndef DEPTH
	DEPTH = ..
endif

include ../includes.mk

OUTDIR = $(OBJDIR)/forkc1-build

ifndef CXX
	CXX=clang++ 
endif

ifndef AR
	AR=ar 
endif

ifndef RANLIB
	RANLIB=ranlib
endif

.PHONY: all,clean

all: dir forkc1 

forkc1: 
	$(CXX) -o $(OBJDIR)/forktree forktree.cc $(LIBS) $(LDFLAGS) $(INCLUDES) -std=gnu++11 -Wall -pedantic
	$(CXX) -o $(OBJDIR)/forkc1 forkc1.cc $(LIBS) $(LDFLAGS) $(INCLUDES) -std=gnu++11 -Wall -pedantic

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/forkc1

dir:
	mkdir -p $(OUTDIR)

