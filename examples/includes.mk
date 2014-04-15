ifndef BPATH
	BPATH = $(shell pwd)
endif

INCLUDES := -I$(BPATH)/../../src/parse/ -I$(BPATH)/../../src/ -I$(BPATH)/../../deps/
OBJDIR := $(BPATH)/../../build/examples
LIBS := -L$(OBJDIR)/../
LDFLAGS := -rdynamic -lparse -llex -lsyms -lutils -ltreemap -llist -larray -g

UNAME = $(shell uname -o)

ifeq ($(UNAME),OpenBSD)
	LDFLAGS += -lbacktrace
endif

ifeq ($(UNAME),FreeBSD)
	LDFLAGS += -lexecinfo
endif


