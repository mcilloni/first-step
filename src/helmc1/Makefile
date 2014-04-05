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

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
OPATHS = $(foreach obj, $(OBJS), $(OUTDIR)/$(obj))
all: dir $(OBJS) helmc1

helmc1: 
	$(CC) -o $(OBJDIR)/helmc1 $(OUTDIR)/helmc1.o $(LIBS) $(LDFLAGS)

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/helmc1

dir:
	mkdir -p $(OUTDIR)

%.o: %.c 
	$(CC) $(INCLUDES) -c $< -o $(OUTDIR)/$@ -std=c11 -Wall -pedantic

