#ifndef RUNTIME_H_
#define RUNTIME_H_

#include <stddef.h>
#include <stdint.h>

#include "ast.h"

typedef struct {
    size_t count;
    size_t capacity;
    int64_t* items;
} Stack;

Stack* stack_new(size_t initial_capacity);
void stack_delete(Stack* stack);
void stack_execute(Stack* stack, const Instructions* instructions);

#endif
