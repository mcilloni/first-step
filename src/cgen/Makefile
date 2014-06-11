ifndef DEPTH
	DEPTH = ..
endif

include ../includes.mk

OUTDIR = $(OBJDIR)/cgen

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
	$(AR) rc $(OBJDIR)/libcgen.a $(OPATHS)
	$(RANLIB) $(OBJDIR)/libcgen.a

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/libcgen.a

dir:
	mkdir -p $(OUTDIR)

%.o: %.cc 
	$(CXX) $(INCLUDES) -c $< -o $(OUTDIR)/$@ -std=gnu++11 -Wall -pedantic

