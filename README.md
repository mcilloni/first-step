first-step
==========

First Step (helmc) is an experimental implementation of a compiler for the Helm programming language, as described in EBNF-like syntax by helm.ebnf.

The main goal of this project is to get a working compiler, written in the C language, capable to bootstrap a future compiler written in Helm itself.
Because of this, certain parts can be poorly written, or can be inefficient. 

What has been done
==================

helmc is capable to build simple programs, with integers and records, and also functions.
It supports pointers and a wide array of c-like operators, with the same behaviour as C.

helmc currently implements:

- integers
- assignments
- arithmetical expressions
- functions
- comments (using the # character) 
- string literals (using "")
- structures (also known as records)
- external declarations of symbols (using the keyword *decl*)

helmc will implement someday:

- type coercion ("casts")
- add debug info to compiled programs
- size operator (to be defined)
- vaguely meaningful diagnostic messages

helmc will not implement:
- syntactical sugar, i.e type inference, declare and assign (var i int8 = 6) and anything not essential to bootstrap the future Helm compiler.
- modules, because they are a complex feature that is not necessary for the sake of bootstrapping a new compiler.
- good diagnostics of any sort

FAQ
=== 

Q: There are random test files between headers and library sources!
A: I know, I have to clean up this mess someday.

Q: How do I compile this?
A: Use GNU Make on the root of the project, or a GNU Make compatible software like pymake.

Q: How can I compile this on VisualStudio?
A: This is C11, and I think MSVC will implement it around the end of the current century, given their pace.

Q: How can I compile this on Windows with MinGW?
A: You don't. I've used POSIX functionality here and there (like backtraces, fmemopen, ..) that Microsoft is not going to support. Wait for a future helm compiler for Windows support.

Q: How can I compile this on Windows with Cygwin?
A: I think this could work on cygwin. Try using CC="gcc" make in a cygwin prompt, you can omit the CC="gcc" part if you have a working clang for cygwin installed.
   You will also need to change "clang" with "gcc" inside firststep.c .

Q: How do I use helmc? 
A: helmc works only in the same directory of helmrt.o and helmc1. It also needs clang, or gcc, or any c compiler.
   Then, you can compile with helmc <file.helm> , creating a <file> executable. 
   If you don't want it to assemble the executable, you can also use the parameter "-c" to create <file.o>.
   
How It Works?
=============

The compiler is structured in this way:

/utils: miscellaneous utilities of some sort
/array, /list, /treemap: this repositories contain some basic data structures used by the compiler. 
/lex: an handwritten lexer. Pretty rought, but works.
/parse: an handwritten LL(1) parser, with some hacks here and there to make it work. Pretty rought too, but it works.
/cgen: code generator for AST created by /parse. It emits C code from the AST (for sake of simplicity) inside a .c file.
/firststep: the actual compiler executable, with a basic runtime. It runs /parse, creates a c file using /cgen and then runs clang.




