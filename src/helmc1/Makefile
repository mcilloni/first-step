ifndef DEPTH
	DEPTH = ..
endif

include ../includes.mk

OUTDIR = $(OBJDIR)/helmc1-build

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

all: dir helmc1 

helmc1: 
	$(CXX) -o $(OBJDIR)/helmtree helmtree.cc $(LIBS) $(LDFLAGS) $(INCLUDES) -std=gnu++11
	$(CXX) -o $(OBJDIR)/helmc1 helmc1.cc $(LIBS) $(LDFLAGS) $(INCLUDES) -std=gnu++11

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/helmc1

dir:
	mkdir -p $(OUTDIR)

