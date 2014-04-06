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

clean:
	rm -rf $(OUTDIR)

dir:
	mkdir -p $(OUTDIR)

%: %.c 
	$(CC) $(INCLUDES) $< -o $(OUTDIR)/$@ -std=c11 -Wall -pedantic $(LIBS) $(LDFLAGS)

