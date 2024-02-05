# Stagma

> A simple stack based programming language.

Educational project to learn about interpreters/compilers for executing source code on a computer.

# Build

- `mkdir build`
- `cd build`
- `cmake ..`
- `cmake --build .`

# Run stagma

Here are some [sample programs](/samples) to run.

## Run interpreter

`stagma <SOURCE-FILE> [...ARGS]`

## Run REPL

`stagma`

## Run compiler

`stagmac -o <ASSEMBLY-FILE> <SOURCE-FILE>`

To assemble and link the program run

`nasm -f elf64 <ASSEMBLY-FILE>`
`ls <OBJECT-FILE>`

These commands can also be run as an one liner for convenience like

`stagmac -o a.s <SOURCE-FILE> && nasm -f elf64 a.s && ld a.o`


# Planed components for toolchain

- [x] Interpreter
- [x] REPL
- [x] Compiler

# Planed changes

- [ ] Merge swap and deref into swap. E.g. pop the distance n, swap the top with item n slots away. 0 swap will be a noop.
      Current swap & deref only allow for read random access. But not write random access
