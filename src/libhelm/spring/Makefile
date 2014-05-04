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

ifeq ($(shell uname -m), x86_64)
ifeq (,$(findstring CYGWIN,$(UNAME)))
	FPIC = -fPIC
endif
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
	@$(CC) -c -o $(OUTDIR)/$@ $< $(FPIC) -std=c11 -D_POSIX_C_SOURCE=200112L

%.helm.o: %.helm
	@echo [HELMC] $<
	@$(HELMC) -o $(OUTDIR)/$@ $<

clean:
	rm -f $(OPATHS)

