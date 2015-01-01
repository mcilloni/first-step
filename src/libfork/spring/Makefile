OUTDIR = ../../../build/libfork

ifndef FORKC
	FORKC=$(OUTDIR)/../forkc
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

HSRCS = $(wildcard *.fork)
CSRCS = $(wildcard *.cc)
HOBJS = $(patsubst %.fork,%.fork.o,$(HSRCS))
COBJS = $(patsubst %.cc,%.cc.o,$(CSRCS))
OPATHS = $(foreach obj, $(HOBJS), $(OUTDIR)/$(obj))
OPATHS += $(foreach obj, $(COBJS), $(OUTDIR)/$(obj))

all: $(COBJS) $(HOBJS)
	mkdir -p $(OUTDIR)

%.cc.o: %.cc
	@echo [CXX] $<
	@$(CXX) -c -g -o $(OUTDIR)/$@ $< $(FPIC) -std=c++14 -D_POSIX_C_SOURCE=200112L

%.fork.o: %.fork
	@echo [FORKC] $<
	@$(FORKC) -o $(OUTDIR)/$@ $<

clean:
	rm -f $(OPATHS)

