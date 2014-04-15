UNAME = $(shell uname)

ifndef RANLIB
	RANLIB = ranlib
endif

all:
ifeq ($(UNAME), OpenBSD)
	$(CC) -c backtrace.c
	$(AR) rc ../../build/libbacktrace.a backtrace.o
	$(RANLIB) ../../build/libbacktrace.a
endif

clean:
	rm backtrace.o
