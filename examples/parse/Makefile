include ../includes.mk

OUTDIR = $(OBJDIR)/$(notdir $(shell pwd))

LDFLAGS += -lutils

ifndef CC
	CC=clang 
endif

.PHONY: all,clean

SRCS = $(wildcard *.c)
BINS = $(patsubst %.c,%,$(SRCS))
OPATHS = $(foreach obj, $(OBJS), $(OUTDIR)/$(obj))
all: dir $(BINS) 
	$(MAKE) -C syms

clean:
	rm -rf $(OUTDIR)
	make -C syms clean

dir:
	mkdir -p $(OUTDIR)

%: %.c 
	$(CC) $(INCLUDES) $< -o $(OUTDIR)/$@ -std=gnu11 -Wall -pedantic $(LIBS) $(LDFLAGS)

