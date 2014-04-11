ifndef DEPTH
	DEPTH = ..
endif


export BPATH=$(shell pwd)/../
include ../../includes.mk

OUTDIR = $(OBJDIR)/parse/syms

ifndef CC
	CC=clang 
endif

AR=ar

ifndef RANLIB
	RANLIB=ranlib
endif

.PHONY: all,clean

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
OPATHS = $(foreach obj, $(OBJS), $(OUTDIR)/$(obj))
all: dir $(OBJS)
	$(AR) -rc $(OBJDIR)/libsyms.a $(OPATHS)
	$(RANLIB) $(OBJDIR)/libsyms.a

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/libsyms.a

dir:
	mkdir -p $(OUTDIR)

%.o: %.c 
	$(CC) $(INCLUDES) -c $< -o $(OUTDIR)/$@ -std=c11 -Wall -pedantic

