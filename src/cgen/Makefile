ifndef DEPTH
	DEPTH = ..
endif

include ../includes.mk

OUTDIR = $(OBJDIR)/cgen

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
	$(AR) -rc $(OBJDIR)/libcgen.a $(OPATHS)
	$(RANLIB) $(OBJDIR)/libcgen.a

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/libcgen.a

dir:
	mkdir -p $(OUTDIR)

%.o: %.c 
	$(CC) $(INCLUDES) -c $< -o $(OUTDIR)/$@ -std=gnu11 -Wall -pedantic

