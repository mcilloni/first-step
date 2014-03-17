ifndef CC
	CC=clang
endif

ifndef RANLIB
	RANLIB=ranlib
endif

ifdef WINDOWS
	EXEPREFIX=.exe
else
	EXEPREFIX=
endif

all: clean
	make -C array
	make -C list
	make -C treemap
	make -C utils
	cp */*.a .
	make -C lex
	make -C parse	
	cp */*.a .

clean:
	make -C array clean
	make -C list clean
	make -C treemap clean
	make -C utils clean
	make -C lex clean
	make -C parse clean
	rm -f *.a
