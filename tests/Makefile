ifndef CC
	CC=clang
endif

HELMC = ../build/helmc

.PHONY: all,clean

SRCS = $(wildcard *.helm)
BINS = $(patsubst %.helm,%,$(SRCS))
all: $(BINS)

clean:
	rm -f $(BINS)

%: %.helm 
	@echo [HELMC] $<
	@$(HELMC) $<  

