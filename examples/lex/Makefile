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

clean:
	rm -rf $(OUTDIR)

dir:
	mkdir -p $(OUTDIR)

%: %.c 
	$(CXX) $(INCLUDES) $< -o $(OUTDIR)/$@ -std=c++14 -Wall -pedantic $(LIBS) $(LDFLAGS)

