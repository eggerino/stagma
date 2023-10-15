#include "runtime.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static int _run_push(Stack* stack, int64_t arg);
static int _run_pop(Stack* stack);
static int _run_dub(Stack* stack);
static int _run_swap(Stack* stack);
static int _run_jump(Stack* stack, int64_t arg, int64_t* current_statement);
static int _run_arithmetic_op(Stack* stack, const char* error_id, int (*op)(int64_t first, int64_t second, int64_t* result));
static int _op_add(int64_t first, int64_t second, int64_t* result);
static int _op_sub(int64_t first, int64_t second, int64_t* result);
static int _op_mul(int64_t first, int64_t second, int64_t* result);
static int _op_div(int64_t first, int64_t second, int64_t* result);
static int _run_print(Stack* stack);
static int _run_debug(const Stack* stack);

int run_start(Stack* stack, int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        int64_t arg = atoll(argv[i]);

        if (stack_push(stack, arg) != STACK_STATUS_OK) {
            fprintf(stderr, "ERROR: [Start] Unable to push %d-th argument %s.\n", i, argv[i]);
            return -1;
        }
    }

    return 0;
}

int run_statement(Stack* stack, Statement* statement, int64_t* current_statement) {
    assert(OP_COUNT == 11 && "Exhaustive switch statement");
    switch (statement->op) {
        case OP_PUSH:
            return _run_push(stack, statement->arg);
        case OP_POP:
            return _run_pop(stack);
        case OP_DUB:
            return _run_dub(stack);
        case OP_SWAP:
            return _run_swap(stack);
        case OP_JUMP:
            return _run_jump(stack, statement->arg, current_statement);
        case OP_ADD:
            return _run_arithmetic_op(stack, "Add", _op_add);
        case OP_SUB:
            return _run_arithmetic_op(stack, "Sub", _op_sub);
        case OP_MUL:
            return _run_arithmetic_op(stack, "Mul", _op_mul);
        case OP_DIV:
            return _run_arithmetic_op(stack, "Div", _op_div);
        case OP_PRINT:
            return _run_print(stack);
        case OP_DEBUG:
            return _run_debug(stack);
    }
}

int run_exit(Stack* stack) {
    int64_t return_code;
    if (stack_pop(stack, &return_code) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Exit] Stack did not end with a value left to return.\n");
        return -1;
    }

    return (int)return_code;
}

int _run_push(Stack* stack, int64_t arg) {
    if (stack_push(stack, arg) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Push] Unable to push the value %d.\n", arg);
        return -1;
    }
    return 0;
}

int _run_pop(Stack* stack) {
    int64_t value;
    if (stack_pop(stack, &value) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Pop] Unable to pop.\n");
        return -1;
    }
    return 0;
}

static int _run_dub(Stack* stack) {
    int64_t value;
    if (stack_pop(stack, &value) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Dub] Unable to pop the value for dublication.\n");
        return -1;
    }

    if (stack_push(stack, value) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Dub] Unable to push the original value %d.\n", value);
        return -1;
    }

    if (stack_push(stack, value) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Dub] Unable to push the copied value %d.\n", value);
        return -1;
    }

    return 0;
}

int _run_swap(Stack* stack) {
    int64_t top, bottom;
    if (stack_pop(stack, &top) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Swap] Unable to pop the top value.\n");
        return -1;
    }

    if (stack_pop(stack, &bottom) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Swap] Unable to pop the bottom value.\n");
        return -1;
    }

    if (stack_push(stack, top) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Swap] Unable to push the new bottom value %d.\n", top);
        return -1;
    }

    if (stack_push(stack, bottom) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Swap] Unable to push the new top value %d.\n", bottom);
        return -1;
    }

    return 0;
}

static int _run_jump(Stack* stack, int64_t arg, int64_t* current_statement) {
    int64_t condition;
    if (stack_pop(stack, &condition) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Jump] Unable to pop the condition value.\n");
        return -1;
    }

    if (condition) {
        *current_statement = arg - 1;  // move to one before the jumped statement so that for the next iteration the correct
                                       // statement gets executed.
    }

    return 0;
}

int _run_arithmetic_op(Stack* stack, const char* error_id, int (*op)(int64_t first, int64_t second, int64_t* result)) {
    int64_t first, second, result;

    if (stack_pop(stack, &first) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [%s] Unable to pop the first operant.\n", error_id);
        return -1;
    }

    if (stack_pop(stack, &second) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [%s] Unable to pop the second operant.\n", error_id);
        return -1;
    }

    int op_status = op(first, second, &result);
    if (op_status) return op_status;

    if (stack_push(stack, result) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [%s] Unable to push the result %d.\n", error_id, result);
        return -1;
    }

    return 0;
}

int _op_add(int64_t first, int64_t second, int64_t* result) {
    *result = first + second;
    return 0;
}

int _op_sub(int64_t first, int64_t second, int64_t* result) {
    *result = second - first;
    return 0;
}

int _op_mul(int64_t first, int64_t second, int64_t* result) {
    *result = first * second;
    return 0;
}

int _op_div(int64_t first, int64_t second, int64_t* result) {
    if (first == 0) {
        fprintf(stderr, "ERROR: [Div] Division by zero is not allowed.\n");
        return -1;
    }
    *result = second / first;
    return 0;
}

int _run_print(Stack* stack) {
    int64_t data;
    if (stack_pop(stack, &data) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Print] Unable to pop the value to print.\n");
        return -1;
    }

    fprintf(stdout, "%d\n", data);
    return 0;
}

int _run_debug(const Stack* stack) {
    fprintf(stderr, "Snapshot of stack:");
    for (size_t i = 0; i < stack->length; ++i) {
        fprintf(stderr, "%d:\t%d\n", i, stack->items[i]);
    }
    return 0;
}
