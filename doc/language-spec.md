# Specification of the stagma language

## General

Stagma is a procedural, stack-based programming language. A global stack of 64 bit integers describes the state of the program.
During execution a list of instructions is run sequentially, which modifies the stack.

## Command line arguments

All command line arguments are parsed to integers and placed at the beginning of the stack in reversed order. After all arguments a count is pushed onto the stack.

```
| arg n | arg n-1 | ... | arg 2 | arg 1 | n |
^                                         ^
|                                         |
|                                         +- Current top value of the stack
+-- Start of the stack
```
With this initial stack the program starts executing.

## End of a program

After executing the last instruction the value currently on top of the stack is the return code of the program. If the stack is empty, a value of zero is implicitly returned.

## Syntax

Any whitespace character or comma is considered a delimiter between instructions. A comment is starts and ends with #. Every character inside a comment is ignored.
A valid instructions is ether an integer literal or one of the keywords in the list of instructions.

## Instructions

### Stack manipulation

#### Push

An integer literal is implicitly used for pushing a value onto the stack. There is no keyword associated with this instruction.

E.g.: `420 -69` pushes 420 and then -69 onto the stack.

#### Pop

`pop`

Remove the current top value from the stack and discard it.

E.g.: `1 2 3 4 pop` pushes 1 to 4 onto the stack and removes the 4.

#### Swap

`swap`

Changes the order of the top two values of the stack.

E.g.: `4 7 swap` pushes 4 and 7 to the stack, with 7 being the top value, then changes their order so 4 becomes the top value.

#### Duplicate

`dup`

Gets the top value of the stack and pushes it to the stack again. The stack contains the value twice after.

E.g.: `4 dup` the stack contains two 4s.

#### Dereference

`deref`

Pops the top value and uses it as a pointer into the stack. The value n indicates which item is referenced by how many items back in the stack it is located relative to the poped value.
The value n is interpreted as an unsigned integer while zero is a self reference. After poping the top value the referenced value is pushed onto the stack.

E.g.:
- `1 2 3 4 0 deref` 1, 2, 3, 4, 0 are pushed. Since 0 is a self reference it is poped and pushed again resulting in a no op.
- `101 102 7 103 104 3 deref` the refernced value is 7. Dereferencing the 3 results in a pop of the 3 and a push of 7.

### IO

#### Print

`print`

Pops the top value and prints it to the standard output.

E.g.: `65 print` will print "A" to the standard output.

#### Error

`err`

Pops the top value and prints it to the standard error.

E.g.: `65 err` will print "A" to the standard output.

#### Input

`input`

Reads from standard input and parses the value. The parsed value is then pushed onto the stack.

E.g.: `3 input` will push 3 and whatever is provided via the standard input.

### Arithmetic

#### Addition

`+`

Pops the two top values, adds them and pushes the result onto the stack.

E.g.: `2 4 +` will add 2 and 4. The stack only contains 6.

#### Subtraction

`-`

Pops the two top values, subtracts the top from the second value and pushes the result onto the stack.

E.g.: `2 4 -` will subtract 4 from 2. The stack only contains -2.

#### Muliplication

`*`

Pops the two top values, multiplies them and pushes the result onto the stack.

E.g.: `2 4 *` will multiply 2 and 4. The stack only contains 8.

#### Division

`/`

Pops the two top values, devides the second value by the top and pushes the result onto the stack.

E.g.: `9 2 /` will devide 9 by 2. The stack only contains 4.

#### Modulo

`%`

Pops the two top values, devides the second value by the top and pushes the remainder onto the stack.

E.g.: `9 2 %` will devide 9 by 2. The stack only contains 1.

#### Power

`^`

Pops the two top values, raises the second value to the power of the top value and pushes the remainder onto the stack.

E.g.: `9 2 ^` will raise 9 to the 2nd power. The stack only contains 81.

### Control flow

#### If

`if ... [else] ... end`

Pops the top value. If the value is non zero, the if instructions execute. If the value is zero, the else instructions execute if any.

E.g.: `65 input if print else err end` will print "A" to the standard output for a non zero input. If a zero is provided, "A" is printed to the standard error.

#### While

`while ... end`

Pops the top value. If the value is non zero, the inner instructions execute and the top value is pop for the comparison of the next iteration. This is repeated until a zero is poped.
The inner block is skipped and the execution continues after the block.

E.g.: `1 while 65 print 1 end` runs a infinte loop and prints "A" in every iteration.

#### Exit

`exit`

Pops the top value and exits the program with the poped value as the return code. If the stack is empty, a zero is returned implicitly.

E.g.: `1 exit 65 print` exits the program with 1 before printing "A".
