ifndef DEPTH
	DEPTH = ..
endif

include ../includes.mk

OUTDIR = $(OBJDIR)/helmc1-build

ifndef CC
	CC=clang 
endif

AR=ar

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
	$(OBJDIR)/helmc1 < helmrt.helm > $(OUTDIR)/helmrt.c
	sed -i 's/int8 main/int main/' $(OUTDIR)/helmrt.c
	$(CC) $(INCLUDES) -c $(OUTDIR)/helmrt.c -o $(OUTDIR)/helmrt.o -std=c11 -w
	rm -f $(OUTDIR)/helmrt.c
