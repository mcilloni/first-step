INCLUDES := -I$(shell pwd)/../ -I$(shell pwd)/../../deps/ -g
OBJDIR := $(shell pwd)/../../build
LIBS := -L$(OBJDIR)
LDFLAGS := -lcgen -lparse -llex -ltreemap -larray -llist -lutils -rdynamic

UNAME = $(shell uname -o)
ifeq ($(UNAME),FreeBSD)
	LDFLAGS += -lexecinfo
endif


