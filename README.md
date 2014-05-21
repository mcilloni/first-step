first-step
==========

First Step (helmc) is an experimental implementation of a compiler for the Helm programming language, as described in EBNF-like syntax by helm.ebnf.

The main goal of this project is to get a working compiler, written in the C language, capable to bootstrap a future compiler written in Helm itself.
Because of this, certain parts could be poorly written, or could be inefficient. 

What has been done
==================

Helmc is capable of compiling quite complex programs, like the linked list and treemap implementations in /src/libhelm/spring.
It also ships a basic, *temporary* library with an expanding set of utilities that will be used for developing second-step, a compiler written in Helm for Helm with, *I hope*, a better parser and a better library.
It supports pointers and a wide array of c-like operators, with the same behaviour as C.
The codebase is still in its early phase, and bugs are frequent.

helmc currently implements:

- integers (int8, int16, int32, int64, uint8, uint16, uint32, uint64, uintptr)
- assignments
- arithmetical expressions (support for unsigned powers with the ^ operator is quite buggy, still)
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
- modules (very, very basic support).

helmc will not implement:

- syntactical sugar, i.e anything not essential to bootstrap the future Helm compiler.
- good diagnostics of any sort

FAQ
=== 

> There are random test files between headers and library sources!

I know, I have to clean up this mess someday.


> How do I compile this?

Use GNU Make on the root of the project, or a GNU Make compatible software like pymake.
This software works on GNU/Linux, FreeBSD, DragonFlyBSD, NetBSD, Darwin (OS X) and Cygwin (32 bit).
The facultative examples (in /examples, buildable with [g]make ex) are not guarranteed to work except on GNU/Linux, but they are not necessary. 


> How do I compile this on Visual Studio?

This is C11, and I think MSVC will implement it around the end of the current century, given their pace.

> How do I compile this on Windows with MinGW?

You don't. I've used GNU/POSIX goodies here and there (like backtraces, fmemopen, ..) that Microsoft is not going to support. Wait for a future helm compiler for Windows support.

> How do I compile this on Windows with Cygwin?

This works on cygwin (32 bit Cygwin, not 64, they changed something that breaks everything), just don't use clang because the Cygwin-shipped version of it is old as hell.
helmc automatically uses gcc instead of clang on Cygwin because of this.

> How do I compile this on OpenBSD?

Work on OpenBSD is still in its early stage. The main issue here is that clang has some issues with code compiled with -g because of old binutils, and default gcc 4.2 is too old. Installing a newer gcc and its binutils with it will fix the clang issues.

> How do I use helmc? 

helmc works only in the same directory of helmrt.o and helmc1. It also needs clang, or gcc, or any c compiler.
Then, you can compile with helmc <file.helm> , creating a <file.o> binary, that you can link with helml <file.o>
Executing helmc -C file.helm generates a file.c inside the current directory.

> How modules work?

Helm modules are .hemd files with decl's of external symbols and aliases of types.
They are, at least now, handwritten. 
helmc1 searches for modules (with *.hemd* extension) in the current directory and in the paths specified into the *HELM_MODULES* variable, separated by ':'. 
All .hemd files have a *module _name_* declaration identical to their file name; per example, spring module has a spring.hemd module with *module spring* at its beginning.
Symbols are not exported with their module name because of limitations of their implementation, so name collision can arise at linking time.
This will be fixed for sure in second-step.

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



