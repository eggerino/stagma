#ifndef STACK_H_
#define STACK_H_

#include <stddef.h>
#include <stdint.h>

typedef struct Stack {
    int64_t* items;
    size_t length;
    size_t capacity;
} Stack;

typedef enum StackStatus {
    STACK_STATUS_OK = 0,
    STACK_STATUS_NO_MEMORY,
    STACK_STATUS_EMPTY,
} StackStatus;

Stack* stack_new(size_t capacity);
void stack_delete(Stack* stack);
StackStatus stack_push(Stack* stack, int64_t data);
StackStatus stack_pop(Stack* stack, int64_t* data);

#endif
