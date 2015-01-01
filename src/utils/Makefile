DEPTH = ..
include ../includes.mk

OUTDIR = $(OBJDIR)/utils

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
	$(AR) rc $(OBJDIR)/libutils.a $(OPATHS)
	$(RANLIB) $(OBJDIR)/libutils.a

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/libutils.a

dir:
	mkdir -p $(OUTDIR)

%.o: %.cc 
	$(CXX) $(INCLUDES) -c $< -o $(OUTDIR)/$@ -std=c++14 -Wall -pedantic

