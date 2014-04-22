first-step
==========

First Step (helmc) is an experimental implementation of a compiler for the Helm programming language, as described in EBNF-like syntax by helm.ebnf.

The main goal of this project is to get a working compiler, written in the C language, capable to bootstrap a future compiler written in Helm itself.
Because of this, certain parts could be poorly written, or could be inefficient. 

What has been done
==================

helmc is capable to build simple programs, with integers and records, and also functions.
It supports pointers and a wide array of c-like operators, with the same behaviour as C.
The codebase is still in its early phase, and bugs are frequent.

helmc currently implements:

- integers (int8, int16, int32, int64, uint8, uint16, uint32, uint64, uintptr)
- assignments
- arithmetical expressions
- functions
- comments (using the # character) 
- string literals (using "")
- structures (also known as records)
- external declarations of symbols (using the keyword *decl*)
- type coercion (through the cast<*type*> operator)
- size(*type*) operator 
- naïve type inference (with assignment)
- bool type with true and false keywords
- null identifier, assignable to every pointer type.
- data type, a special pointer type capable of holding a pointer of any type (like C pointers to void) 
- add debug info to compiled programs 

helmc will implement someday:

- vaguely meaningful diagnostic messages

helmc will not implement:

- syntactical sugar, i.e anything not essential to bootstrap the future Helm compiler.
- modules, because they are a complex feature that is not necessary for the sake of bootstrapping a new compiler.
- good diagnostics of any sort

FAQ
=== 

> There are random test files between headers and library sources!

I know, I have to clean up this mess someday.


> How do I compile this?

Use GNU Make on the root of the project, or a GNU Make compatible software like pymake.
This software works on GNU/Linux and FreeBSD (and any FBSD derivative like DragonFlyBSD and - I hope - Darwin aka OS X).


> How do I compile this on Visual Studio?

This is C11, and I think MSVC will implement it around the end of the current century, given their pace.

> How do I compile this on Windows with MinGW?

You don't. I've used POSIX functionality here and there (like backtraces, fmemopen, ..) that Microsoft is not going to support. Wait for a future helm compiler for Windows support.

> How do I compile this on Windows with Cygwin?

This works on cygwin (32 bit Cygwin, not 64, they changed something that breaks everything), just don't use clang because the Cygwin-shipped version of it is old as hell.
helmc automatically uses gcc instead of clang on Cygwin because of this.

> How do I compile this on OpenBSD?

Work on OpenBSD is still in its early stage. The main issue here is that clang has some issues with code compiled with -g because of old binutils, and default gcc 4.2 is too old.

> How do I use helmc? 

helmc works only in the same directory of helmrt.o and helmc1. It also needs clang, or gcc, or any c compiler.
Then, you can compile with helmc <file.helm> , creating a <file> executable. 
If you don't want it to assemble the executable, you can also use the parameter "-c" to create <file.o>.
Executing helmc -C file.helm generates a file.c inside the current directory.
   
How It Works?
=============

The compiler is structured in this way:

- /src: main source repository.
- /src/lex: an handwritten lexer. Pretty rought, but works.
- /src/parse: an handwritten LL(1) parser, with some hacks here and there to make it work. Pretty rought too, but it works.
- /src/cgen: code generator for AST created by /parse. It emits C code from the AST (for sake of simplicity) inside a .c file.
- /src/helmc1: the actual compiler executable, which behaves like cc1 of gcc (some sort of). It takes Helm input of some sort and outputs C code.
- /src/utils: miscellaneous utilities of some sort
- /src/helmc.sh: a Bash script which is used to create /build/helmc. This script takes care of invocation of helmc1 and compiling of the generated C code.
- /deps: contains some basic data structures used by the compiler, like array, list and treemap, and a backtrace lib for OpenBSD.
- /examples: examples and basic tests about the utils, lex and parse libraries. This contains the utility helmtree and can be built using make ex (or gmake on BSD)
- /tests: sample Helm programs.

The compiler itself is a simple C executable that takes an input file and outputs C code called helmc1. 
Helmc1 is wrapped by helmc, that is a bash script that takes care of finding the right C compiler and invoke it, plus it is able to handle c-compiler style switches that helmc1 does not support. 



