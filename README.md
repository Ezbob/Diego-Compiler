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

### Type

The keyword `type` can be used to define a type. E.g. `type id = int;` would alias `id` to the `int` data type.

### Collections

Diego supports `arrays`, and `records`. To define an array, prefix the type with `array of`, e.g.

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

Records can be defined using the `type` keyword, e.g. for creation of binary trees:

```
type Node = record { leftChild: Node, rightChild: Node, key: int };
```

Which defines a node of a binary tree.

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

## Comparisons

Diego has `true` and `false` as boolean constants, and `null` for usage with records or arrays.

Equality operators in Diego are similar to most other imperative languages, being `==` for equality, `!=` for inequality.
Additionally, the language has `<`, `>`, `<=`, `>=` for equality checks.

### Control flow

#### If-statements

If-statements in Diego can be followed a else-statement, or have the else omitted. The predicate must be enclosed by
a parenthesis. Predicates can be chained using the `&&` or the `||` operators.

```
if (i == 0) then write 1;
else write 0; // Can be omitted
```

If-statements can contain either a single statement or statement list (enclosed by curly braces).

#### While loops

While loops syntax require a predicate, similar to if statements, but the predicate is followed by the keyword `do`, e.g.:

```
var i: int;
i = 5;
while (i < 5) do write i;
```

The body of a while statement can either be a single statement, or list of statements.

### Example - Binary tree traversal

```
type Node = record of { leftChild: Node, rightChild: Node, key: int };
func initNode(key: int, leftChild: Node, rightChild: Node): Node
    var n: Node;
    n.key = key;
    n.leftChild = leftChild;
    n.rightChild = rightChild;
    return n;
end initNode

var leftChild: Node, rightChild: Node, root: Node;
leftChild = initNode(1, null, null);
rightChild = initNode(2, null, null);
root = initNode(0, leftChild, rightChild);

func sumKeys(node: Node): int
    var keyValue: int;
    keyValue = 0;
    if (node.leftChild != null) then keyValue = keyValue + sumKeys(node.leftChild);
    if (node.rightChild != null) then keyValue = keyValue + sumKeys(node.rightChild);
    return keyValue + node.key;
end sumKeys

write sumKeys(root); // 3
```


## Contributors
The main contributors are:
- Anders Busch <andersbusch@gmail.com>
- Henrik K. Clausen
