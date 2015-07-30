ifndef BPATH

	BPATH = $(shell pwd)

endif

UNAME = $(shell uname)
MACHINE = $(shell uname -m)

INCLUDES := -I$(BPATH)/../ -I$(BPATH)/../parse/ -I$(BPATH)/../../deps/ -g
OBJDIR := $(BPATH)/../../build
LIBS := -L$(OBJDIR)
LDFLAGS := -lcgen -lparse -llex -lsyms -ltreemap -larray -llist -lutils

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

ifeq ($(MACHINE), x86_64)
	AMD64 = yes
endif


ifeq ($(MACHINE), amd64)
	AMD64 = yes
endif

ifeq (,$(findstring CYGWIN,$(UNAME)))
	LDFLAGS += -rdynamic
ifdef AMD64
	INCLUDES += -fPIC
endif
endif
