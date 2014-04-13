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

all: dir helmc1 helmrt.o

helmc1: 
	$(CC) -o $(OBJDIR)/helmc1 helmc1.c $(LIBS) $(LDFLAGS) $(INCLUDES)

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/helmc1

dir:
	mkdir -p $(OUTDIR)

helmrt.o:  
ifneq (,$(findstring LLVM,$(shell file $(OBJDIR)/helmc1)))
		lli $(OBJDIR)/helmc1 < helmrt.helm > $(OUTDIR)/helmrt.c
else
		$(OBJDIR)/helmc1 < helmrt.helm > $(OUTDIR)/helmrt.c
endif
	$(CC) $(INCLUDES) -c $(OUTDIR)/helmrt.c -o $(OUTDIR)/helmrt.o -w
	rm -f $(OUTDIR)/helmrt.c
