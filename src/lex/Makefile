ifndef DEPTH
	DEPTH = ..
endif

include ../includes.mk

OUTDIR = $(OBJDIR)/lex

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
	$(AR) rc $(OBJDIR)/liblex.a $(OPATHS)
	$(RANLIB) $(OBJDIR)/liblex.a

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/liblex.a

dir:
	mkdir -p $(OUTDIR)

%.o: %.cc 
	$(CXX) $(INCLUDES) -c $< -o $(OUTDIR)/$@ -std=c++14 -Wall -pedantic

