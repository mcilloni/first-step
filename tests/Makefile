ifndef CXX
	CXX=clang++
endif

FORKC = ../build/forkc
FORKL = ../build/forkl

.PHONY: all,clean

SRCS = $(wildcard *.fork)
OBJS = $(patsubst %.fork,%.o,$(SRCS))
BINS = $(patsubst %.fork,%.elf,$(SRCS))
all: $(BINS)

clean:
	rm -f $(BINS) $(OBJS)

%.elf: %.o
	@echo [FORKL] $<
	@$(FORKL) -o $@ $<

%.o: %.fork 
	@echo [FORKC] $<
	@$(FORKC) $<  

