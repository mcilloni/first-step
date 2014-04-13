DEPTH = ..
include ../includes.mk

OUTDIR = $(OBJDIR)/utils

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
	$(AR) rc $(OBJDIR)/libutils.a $(OPATHS)
	$(RANLIB) $(OBJDIR)/libutils.a

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/libutils.a

dir:
	mkdir -p $(OUTDIR)

%.o: %.c 
	$(CC) $(INCLUDES) -c $< -o $(OUTDIR)/$@ -std=c11 -Wall -pedantic

