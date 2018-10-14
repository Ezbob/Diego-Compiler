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

#### Required tools
- make build tool
- flex scanner generator tool
- bison parser generator tool

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

## The Diego Language

### Data types
 
The primitive data types available in Diego consists of `int`, `bool`. Defining a variable is done using the `var` keyword, e.g.:

```
var a: int;
```

As seen above, a statement is terminated with a `;`.

Multiple variables can be instantiated on the same line, by separating using a comma, as follows:

```
var a: int, b: bool;
```

### Collections

DanskDiego supports `arrays`, and `records`. To define an array, prefix the type with `array of`, e.g.

```
var a: array of int, b: array of bool, c: array of array of bool;
```

Where `c` is a two-dimensional array.

As the code reads, no size is defined for the arrays. This is done by using the `allocate` keyword. Using the `allocate` 
keyword requires use of a size, given as an integer, e.g.:

```
allocate b of length 5;
```

Which in conjunction with the previous snippet allocate `b` as an array of booleans of size 5.

Accessing values in an array are done using brackets. Accessing the 0th index in an array would be written as `arr[0]`. 

A record is similar to a C-struct, and uses the keywords `record of` to define. An example:

```
var rec: record of { a: int, b: bool };
```

Which creates a structure with the children being accessible through `.`, e.g. `rec.a` or `rec.b`.

Like arrays, records will need to be allocated, which is done similarly to arrays, but without the size:

```
var rec: record of { a: int, b: bool };
allocate rec;
```

### Functions

Defining a function is done using the `func` keyword. A function must have a name and a return type, and be terminated 
by `end` followed by the function name.

A function returning an integer would be defined like this:

```
func returnInteger(a: int): int
    return a;
end returnInteger
```

which defines the function `returnInteger` which takes a single argument, an integer.

Calling the function is done like most C-like programming languages:

```
returnInteger(5);
```

### Array length and absolute value

To retrieve the length of an array, the pipe operator can be used:

```
var length: int, arr: array of int;
allocate arr of length 5;
length = |arr|;
write length; // 5
```

Additionally, the pipe operator can also be used on an integer to get the absolute value, e.g:

```
var a: int;
a = 0-5;
write |a|; // 5
```


## Contributors
The main contributors are:
- Anders Busch <andersbusch@gmail.com>
- Henrik K. Clausen
