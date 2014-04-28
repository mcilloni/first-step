OUTDIR = ../../../build/libhelm

ifndef HELMC
	HELMC=$(OUTDIR)/../helmc
endif

ifndef CC
	CC=clang
endif

ifndef AR
	AR=ar
endif

ifndef RANLIB
	RANLIB=ranlib
endif

.PHONY: all clean

HSRCS = $(wildcard *.helm)
CSRCS = $(wildcard *.c)
HOBJS = $(patsubst %.helm,%.helm.o,$(HSRCS))
COBJS = $(patsubst %.c,%.c.o,$(CSRCS))
OPATHS = $(foreach obj, $(HOBJS), $(OUTDIR)/$(obj))
OPATHS += $(foreach obj, $(COBJS), $(OUTDIR)/$(obj))

all: $(COBJS) $(HOBJS)
	mkdir -p $(OUTDIR)

%.c.o: %.c
	@echo [CC] $<
	@$(CC) -c -o $(OUTDIR)/$@ $<

%.helm.o: %.helm
	@echo [HELMC] $<
	@$(HELMC) -o $(OUTDIR)/$@ $<

clean:
	rm -f $(OPATHS)

