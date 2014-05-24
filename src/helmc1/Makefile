ifndef DEPTH
	DEPTH = ..
endif

include ../includes.mk

OUTDIR = $(OBJDIR)/helmc1-build

ifndef CC
	CC=clang 
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
	$(CC) -o $(OBJDIR)/helmtree helmtree.c $(LIBS) $(LDFLAGS) $(INCLUDES) -std=gnu11
	$(CC) -o $(OBJDIR)/helmc1 helmc1.c $(LIBS) $(LDFLAGS) $(INCLUDES) -std=gnu11

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/helmc1

dir:
	mkdir -p $(OUTDIR)

