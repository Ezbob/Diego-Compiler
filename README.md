# Diego-Compiler

## What is this?
This is a compiler for a simple language called Diego. The Diego language is a toy procedural programming language made-up to satisfy the requires of my BA project.  

## What does it do?
The goal of the Diego compiler is to take any legal Diego program and translate the Diego code into x86 assembler code. As such if your computer cannot execute x86 assembler code, then this compiler won't work for you (that is, you can compile Diego programs but you can't execute them).

### Language and compiler used
The compiler is written in C, and was compiled using the "gcc" compiler (version 4.8.4).

### Version
Version 1.0.0

## How-to
### Compiling the source
As the project uses the make tool to compile the whole project the following shell command should give you a executable named "kitty":
``` bash
make
```
To remove the executable and all of the object files we have:
``` bash
make clean
```
### Using the compiler
The compiler uses the stdin and stdout streams to take input and produce output to the user. As such, you can use redirecting to produce the x86 assembler code:
``` bash
./kitty < source_file.die > assembler_file.s
```
Here we use the extendsions ".die" for Diego source files and ".s" for x86 assembler files. 
### Producing an executable
Once we have the assembler file, this has to be assembler. To do this we can use the GNU assembler that is included in the GNU C compiler:
``` bash
./gcc -m32 assembler_file.s -o executable_file
```
This will create a executable file that you can have fun with.

## Contributors
The main contributors are:
- Anders Busch <andersbusch@gmail.com>
- Henrik K. Clausen
