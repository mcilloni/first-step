ifndef DEPTH
	DEPTH = ..
endif

include ../includes.mk

OUTDIR = $(OBJDIR)/parse

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

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
OPATHS = $(foreach obj, $(OBJS), $(OUTDIR)/$(obj))
all: dir $(OBJS)
	$(AR) rc $(OBJDIR)/libparse.a $(OPATHS)
	$(RANLIB) $(OBJDIR)/libparse.a
	$(MAKE) -C syms

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/libparse.a
	$(MAKE) -C syms clean

dir:
	mkdir -p $(OUTDIR)

%.o: %.c 
	$(CC) $(INCLUDES) -c $< -o $(OUTDIR)/$@ -std=gnu11 -Wall -pedantic 

