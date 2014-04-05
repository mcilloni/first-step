INCLUDES := -I$(shell pwd)/../ -I$(shell pwd)/../../deps/
OBJDIR := $(shell pwd)/../../build
LIBS := -L$(OBJDIR)
LDFLAGS := -lcgen -lparse -llex -ltreemap -larray -llist -lutils 

UNAME = $(shell uname -o)
ifeq ($(UNAME),FreeBSD)
	LDFLAGS += -lexecinfo
endif


