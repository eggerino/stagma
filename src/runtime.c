#include "runtime.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "dynamic_array.h"

#define _stack_top(stack) (stack).items[(stack).count - 1]

static void _execute_push(Stack* restrict stack, const PushInstructionContext* restrict context);
static void _execute_pop(Stack* stack);
static void _execute_swap(Stack* stack);
static void _execute_dup(Stack* stack);
static void _execute_deref(Stack* stack);

static void _execute_print(Stack* stack);
static void _execute_err(Stack* stack);
static void _execute_input(Stack* stack);

static void _execute_add(Stack* stack);
static void _execute_sub(Stack* stack);
static void _execute_mul(Stack* stack);
static void _execute_div(Stack* stack);
static void _execute_mod(Stack* stack);
static void _execute_pow(Stack* stack);

static void _execute_if(Stack* restrict stack, const IfInstructionContext* restrict context);
static void _execute_while(Stack* restrict stack, const WhileInstructionContext* restrict context);
static void _execute_exit(Stack* stack);

Stack* stack_new(size_t initial_capacity) {
    Stack* stack = (Stack*)calloc(1, sizeof(Stack));
    assert(stack && "Unable to initialize stack");

    stack->capacity = initial_capacity;
    stack->items = (int64_t*)malloc(initial_capacity * sizeof(int64_t));
    assert(stack->items && "Unable to initialize stack memory");

    return stack;
}

void stack_delete(Stack* stack) {
    free(stack->items);
    free(stack);
}

void stack_execute(Stack* restrict stack, const Instructions* restrict instructions) {
    for (size_t i = 0; i < instructions->count; ++i) {
        static_assert(COUNT_INST == 17);  // Exhaustive pattern matching
        switch (instructions->items[i].type) {
            // Stack manipulation
            case INST_PUSH:
                _execute_push(stack, &instructions->items[i].context.push_instruction);
                break;
            case INST_POP:
                _execute_pop(stack);
                break;
            case INST_SWAP:
                _execute_swap(stack);
                break;
            case INST_DUP:
                _execute_dup(stack);
                break;
            case INST_DEREF:
                _execute_deref(stack);
                break;

            // IO
            case INST_PRINT:
                _execute_print(stack);
                break;
            case INST_ERR:
                _execute_err(stack);
                break;
            case INST_INPUT:
                _execute_input(stack);
                break;

            // Artithmetic
            case INST_ADD:
                _execute_add(stack);
                break;
            case INST_SUB:
                _execute_sub(stack);
                break;
            case INST_MUL:
                _execute_mul(stack);
                break;
            case INST_DIV:
                _execute_div(stack);
                break;
            case INST_MOD:
                _execute_mod(stack);
                break;
            case INST_POW:
                _execute_pow(stack);
                break;

            // Control flow
            case INST_IF:
                _execute_if(stack, &instructions->items[i].context.if_instruction);
                break;

            case INST_WHILE:
                _execute_while(stack, &instructions->items[i].context.while_instruction);
                break;

            case INST_EXIT:
                _execute_exit(stack);
                break;

            default:
                assert(0 && "Unreachable");
                break;
        }
    }
}

void _execute_push(Stack* restrict stack, const PushInstructionContext* restrict context) {
    da_push(*stack, context->literal);
}

void _execute_pop(Stack* stack) {
    assert(stack->count >= 1 && "At least one item needs to be on the stack to pop");
    da_pop(*stack);
}

void _execute_swap(Stack* stack) {
    assert(stack->count >= 2 && "At least two items need to be on the stack to swap");

    int64_t temp = _stack_top(*stack);
    _stack_top(*stack) = stack->items[stack->count - 2];
    stack->items[stack->count - 2] = temp;
}

void _execute_dup(Stack* stack) {
    assert(stack->count >= 1 && "At least one item needs to be on the stack to dup");

    da_push(*stack, _stack_top(*stack));
}

void _execute_deref(Stack* stack) {
    assert(stack->count >= 1 && "At least one item needs to be on the stack to deref");

    size_t index_top_value = stack->count - 1;
    int64_t top_value = stack->items[index_top_value];

    uint64_t offset = *((uint64_t*)&top_value);  // Reinterprete as unsigned

    assert(index_top_value >= offset && "Pointer of deref points out of the stack");
    size_t index_derefed_value = index_top_value - offset;

    int64_t derefed_value = stack->items[index_derefed_value];

    stack->items[index_top_value] = derefed_value;
}

void _execute_print(Stack* stack) {
    assert(stack->count >= 1 && "At least one item needs to be on the stack to print");

    char character = _stack_top(*stack);
    da_pop(*stack);

    fprintf(stdout, "%c", character);
}

void _execute_err(Stack* stack) {
    assert(stack->count >= 1 && "At least one item needs to be on the stack to err");

    char character = _stack_top(*stack);
    da_pop(*stack);

    fprintf(stderr, "%c", character);
}

void _execute_input(Stack* stack) {
    int64_t input;
    int result = scanf("%lld", &input);

    assert(result == 1 && "Could not read value from stdin to input");

    da_push(*stack, input);
}

void _execute_add(Stack* stack) {
    assert(stack->count >= 2 && "At least two items needs to be on the stack to add");

    int64_t summand = _stack_top(*stack);
    da_pop(*stack);

    _stack_top(*stack) += summand;
}

void _execute_sub(Stack* stack) {
    assert(stack->count >= 2 && "At least two items needs to be on the stack to sub");

    int64_t subtrahend = _stack_top(*stack);
    da_pop(*stack);

    _stack_top(*stack) -= subtrahend;
}

void _execute_mul(Stack* stack) {
    assert(stack->count >= 2 && "At least two items needs to be on the stack to mul");

    int64_t factor = _stack_top(*stack);
    da_pop(*stack);

    _stack_top(*stack) *= factor;
}

void _execute_div(Stack* stack) {
    assert(stack->count >= 2 && "At least two items needs to be on the stack to div");

    int64_t divisor = _stack_top(*stack);
    da_pop(*stack);

    _stack_top(*stack) /= divisor;
}

void _execute_mod(Stack* stack) {
    assert(stack->count >= 2 && "At least two items needs to be on the stack to mod");

    int64_t divisor = _stack_top(*stack);
    da_pop(*stack);

    _stack_top(*stack) %= divisor;
}

void _execute_pow(Stack* stack) {
    assert(stack->count >= 2 && "At least two items needs to be on the stack to pow");

    int64_t exponent = _stack_top(*stack);
    da_pop(*stack);

    int64_t base = _stack_top(*stack);

    int64_t result = (int64_t)(pow(base, exponent) + 0.5);  // only floating point arithmetic si provided by math.h

    _stack_top(*stack) = result;
}

void _execute_if(Stack* restrict stack, const IfInstructionContext* restrict context) {
    assert(stack->count >= 1 && "At least one item needs to be on the stack to evaluate the confiditon of the if");

    int64_t condition = _stack_top(*stack);
    da_pop(*stack);

    if (condition) {
        stack_execute(stack, &context->if_block);
    } else {
        stack_execute(stack, &context->else_block);
    }
}

void _execute_while(Stack* restrict stack, const WhileInstructionContext* restrict context) {
    assert(stack->count >= 1 && "At least one item needs to be on the stack to evaluate the confiditon of the while");

    int64_t condition = _stack_top(*stack);
    da_pop(*stack);

    while (condition) {
        stack_execute(stack, &context->inner_block);

        assert(stack->count >= 1 && "At least one item needs to be on the stack to evaluate the confiditon of the while");
        condition = _stack_top(*stack);
        da_pop(*stack);
    }
}

void _execute_exit(Stack* stack) {
    if (stack->count == 0) {
        exit(0);
    }

    exit(_stack_top(*stack));
}
