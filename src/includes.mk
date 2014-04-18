ifndef BPATH

	BPATH = $(shell pwd)

endif

UNAME = $(shell uname)

INCLUDES := -I$(BPATH)/../ -I$(BPATH)/../parse/ -I$(BPATH)/../../deps/ -g
OBJDIR := $(BPATH)/../../build
LIBS := -L$(OBJDIR)
LDFLAGS := -lcgen -lparse -llex -lsyms -ltreemap -larray -llist -lutils -rdynamic

ifeq ($(UNAME),NetBSD)
	LDFLAGS += -lmemstream
endif

ifeq ($(UNAME),OpenBSD)
	LDFLAGS += -lbacktrace
endif

ifeq ($(UNAME),FreeBSD)
	LDFLAGS += -lexecinfo
endif

ifeq ($(UNAME),Darwin)
	LDFLAGS += -lmemstream
endif


