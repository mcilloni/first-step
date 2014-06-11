OUTDIR = ../../../build/libhelm

ifndef HELMC
	HELMC=$(OUTDIR)/../helmc
endif

ifndef CXX
	CXX=clang++
endif

ifndef AR
	AR=ar
endif

ifndef RANLIB
	RANLIB=ranlib
endif

MACHINE = $(shell uname -m)

ifeq ($(MACHINE), x86_64)
	AMD64 = yes
endif

ifeq ($(MACHINE), amd64)
	AMD64 = yes
endif

ifdef AMD64
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
	@echo [CXX] $<
	@$(CXX) -c -o $(OUTDIR)/$@ $< $(FPIC) -std=c++11 -D_POSIX_C_SOURCE=200112L

%.helm.o: %.helm
	@echo [HELMC] $<
	@$(HELMC) -o $(OUTDIR)/$@ $<

clean:
	rm -f $(OPATHS)

