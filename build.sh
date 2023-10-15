#!/usr/bin/bash

# Build directory
mkdir -p bin

# Compiler
gcc -g -o bin/stagmac stagmac.c parser.c

# Runtime
gcc -g -o bin/stagmar runtime.c stack.c stagmar.c
