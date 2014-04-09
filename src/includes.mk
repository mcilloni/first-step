ifndef BPATH

	BPATH = $(shell pwd)

endif

INCLUDES := -I$(BPATH)/../ -I$(BPATH)/../parse/ -I$(BPATH)/../../deps/ -g
OBJDIR := $(BPATH)/../../build
LIBS := -L$(OBJDIR)
LDFLAGS := -lcgen -lparse -llex -lsyms -ltreemap -larray -llist -lutils -rdynamic

UNAME = $(shell uname -o)
ifeq ($(UNAME),FreeBSD)
	LDFLAGS += -lexecinfo
endif


