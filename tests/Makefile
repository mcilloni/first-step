ifndef CXX
	CXX=clang++
endif

FORKC = ../build/forkc
FORKL = ../build/forkl

.PHONY: all,clean

SRCS = $(wildcard *.fork)
OBJS = $(patsubst %.fork,%.o,$(SRCS))
BINS = $(patsubst %.fork,%,$(SRCS))
all: $(BINS)

clean:
	rm -f $(BINS) $(OBJS)

%: %.o
	@echo [FORKL] $<
	@$(FORKL) $<

%.o: %.fork 
	@echo [FORKC] $<
	@$(FORKC) $<  

