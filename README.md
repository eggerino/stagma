# About

A simple stack based programming language.
Educational project to learn about compilers/interpreters for executing source code on a computer.

# Run example

## With runtime

- Build the toolchain `./build.sh`
- Compile example to byte code `./bin/stagmac bytecode example/test.sm bin/test.smb`
- Run the byte code in the runtime `./bin/stagmar bin/test.smb`


# Planed components for toolchain

- [x] Parser
- [x] Runtime
- [ ] Interpreter
- [ ] REPL
- [ ] Compiler
