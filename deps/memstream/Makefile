UNAME = $(shell uname)

ifndef RANLIB
	RANLIB = ranlib
endif

ifeq ($(UNAME), NetBSD)
	NEEDED =
endif

ifeq ($(UNAME), Darwin)
	NEEDED = 
endif

all:
ifdef NEEDED
	$(CC) -c memstream.c -Wall -pedantic
	$(AR) rc ../../build/libmemstream.a memstream.o
	$(RANLIB) ../../build/libmemstream.a
endif

clean:
	rm -f memstream.o
