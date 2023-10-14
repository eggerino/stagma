#!/usr/bin/bash

mkdir -p bin

gcc -g -o bin/stagmar runtime.c stack.c stagmar.c