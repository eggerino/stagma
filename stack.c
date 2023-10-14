#include "stack.h"

#include <stdlib.h>
#include <string.h>

typedef enum ReallocState { OK, FAILED } ReallocState;

static ReallocState _stack_realloc(Stack* stack, size_t new_capacity);

Stack* stack_new(size_t capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    Data* items = (Data*)malloc(sizeof(Data) * capacity);

    if (stack == NULL || items == NULL) {
        free(stack);
        free(items);
        return NULL;
    }

    stack->items = items;
    stack->capacity = capacity;
    stack->length = 0;

    return stack;
}

void stack_delete(Stack* stack) {
    if (stack != NULL) {
        free(stack->items);
    }
    free(stack);
}

StackStatus stack_push(Stack* stack, const Data* data) {
    if (stack->length >= stack->capacity) {
        if (_stack_realloc(stack, 2 * stack->capacity) != OK) {
            return STACK_STATUS_NO_MEMORY;
        }
    }

    stack->items[stack->length++] = *data;
    return STACK_STATUS_OK;
}

StackStatus stack_pop(Stack* stack, Data* data) {
    if (stack->length == 0) {
        return STACK_STATUS_EMPTY;
    }

    *data = stack->items[stack->length-- - 1];

    if (stack->length < stack->capacity / 4) {
        if (_stack_realloc(stack, stack->capacity / 4) != OK) {
            return STACK_STATUS_NO_MEMORY;
        }
    }

    return STACK_STATUS_OK;
}

ReallocState _stack_realloc(Stack* stack, size_t new_capacity) {
    Data* new_items = (Data*)malloc(sizeof(Data) * new_capacity);
    if (new_items == NULL) {
        return FAILED;
    }

    // Only lesser amount needs to be copied
    // size increase -> padding with garbo
    // size decrease -> only garbo in unused stack
    size_t num_to_copy = stack->capacity < new_capacity ? stack->capacity : new_capacity;
    memcpy(new_items, stack->items, sizeof(Data) * num_to_copy);
    free(stack->items);

    stack->capacity = new_capacity;
    stack->items = new_items;
    return OK;
}
