INCLUDES := -I$(shell pwd)/../../src/ -I$(shell pwd)/../../deps/
OBJDIR := $(shell pwd)/../../build/examples
LIBS := -L$(OBJDIR)/../
LDFLAGS := -rdynamic -lparse -llex -lutils -ltreemap -llist -larray -g

UNAME = $(shell uname -o)
ifeq ($(UNAME),FreeBSD)
	LDFLAGS += -lexecinfo
endif


