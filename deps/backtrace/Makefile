UNAME = $(shell uname)

all:
ifeq ($(UNAME), OpenBSD)
	cp ../btrace-obsd/libbacktrace/backtrace.c .
	cp ../btrace-obsd/libbacktrace/backtrace.h .
	$(CC) -c backtrace.c
	$(AR) rc ../../build/libbacktrace.a backtrace.o
	$(RANLIB) ../../build/libbacktrace.a
endif

clean:
	rm *
