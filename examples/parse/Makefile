include ../includes.mk

OUTDIR = $(OBJDIR)/$(notdir $(shell pwd))

LDFLAGS += -lutils

ifndef CXX
	CXX=clang++ 
endif

.PHONY: all,clean

SRCS = $(wildcard *.c)
BINS = $(patsubst %.c,%,$(SRCS))
OPATHS = $(foreach obj, $(OBJS), $(OUTDIR)/$(obj))
all: dir $(BINS) 
	$(MAKE) -C syms

clean:
	rm -rf $(OUTDIR)
	$(MAKE) -C syms clean

dir:
	mkdir -p $(OUTDIR)

%: %.c 
	$(CXX) $(INCLUDES) $< -o $(OUTDIR)/$@ -std=gnu++14 -Wall -pedantic $(LIBS) $(LDFLAGS)

