ifndef CC
	CC=clang
endif

ifndef RANLIB
	RANLIB=ranlib
endif

all: clean
	$(MAKE) -C array
	$(MAKE) -C list
	$(MAKE) -C treemap
	install -d ../build/
	cp */*.a ../build

clean:
	$(MAKE) -C array clean
	$(MAKE) -C list clean
	$(MAKE) -C treemap clean
	rm -f ../build/array.a
	rm -f ../build/list.a
	rm -f ../build/treemap.a
