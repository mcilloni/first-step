ifndef CXX
	CXX=clang++
endif

HELMC = ../build/helmc
HELML = ../build/helml

.PHONY: all,clean

SRCS = $(wildcard *.helm)
OBJS = $(patsubst %.helm,%.o,$(SRCS))
BINS = $(patsubst %.helm,%,$(SRCS))
all: $(BINS)

clean:
	rm -f $(BINS) $(OBJS)

%: %.o
	@echo [HELML] $<
	@$(HELML) $<

%.o: %.helm 
	@echo [HELMC] $<
	@$(HELMC) $<  

