ifndef DEPTH
	DEPTH = ..
endif

include ../includes.mk

OUTDIR = $(OBJDIR)/lex

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
all: dir $(OBJS)
	$(AR) -rc $(OBJDIR)/liblex.a $(OPATHS)
	$(RANLIB) $(OBJDIR)/liblex.a

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJDIR)/liblex.a

dir:
	mkdir -p $(OUTDIR)

%.o: %.c 
	$(CC) $(INCLUDES) -c $< -o $(OUTDIR)/$@ -std=c11 -Wall -pedantic

