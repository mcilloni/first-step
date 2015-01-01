ifndef DEPTH
	DEPTH = ..
endif


export BPATH=$(shell pwd)/../
include ../../includes.mk

OUTDIR = $(OBJDIR)/parse/syms

ifndef CXX
	CXX=clang++ 
endif

ifndef AR
	AR=ar
endif

ifndef RANLIB
	RANLIB=ranlib
endif

.PHONY: all,clean

SRCS = $(wildcard *.cc)
OBJS = $(patsubst %.cc,%.o,$(SRCS))
OPATHS = $(foreach obj, $(OBJS), $(OUTDIR)/$(obj))
all: dir $(OBJS)
	$(AR) rc $(OBJDIR)/libsyms.a $(OPATHS)
	$(RANLIB) $(OBJDIR)/libsyms.a

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/libsyms.a

dir:
	mkdir -p $(OUTDIR)

%.o: %.cc 
	$(CXX) $(INCLUDES) -c $< -o $(OUTDIR)/$@ -std=c++14 -Wall -pedantic

