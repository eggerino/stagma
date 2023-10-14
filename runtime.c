#include "runtime.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

static int _exec_push(Stack* stack, const Data* arg);
static int _exec_binary_op(Stack* stack, char* error_id, int (*op)(const Data* first, const Data* second, Data* result));
static int _exec_print(Stack* stack);
static int _op_add(const Data* first, const Data* second, Data* result);
static int _op_sub(const Data* first, const Data* second, Data* result);
static int _op_mul(const Data* first, const Data* second, Data* result);
static int _op_div(const Data* first, const Data* second, Data* result);

int run_start(Stack* stack, int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        Data data = { .as_int64=atoll(argv[i])};
        int status = _exec_push(stack, &data);
        if (status) return status;
    }

    return 0;
}

int run_statement(Stack* stack, Statement* statement) {
    assert(OP_COUNT == 6 && "Exhaustive switch statement");
    switch (statement->op) {
        case OP_PUSH:
            return _exec_push(stack, &statement->arg);
        case OP_ADD:
            return _exec_binary_op(stack, "Add", _op_add);
        case OP_SUB:
            return _exec_binary_op(stack, "Sub", _op_sub);
        case OP_MUL:
            return _exec_binary_op(stack, "Mul", _op_mul);
        case OP_DIV:
            return _exec_binary_op(stack, "Div", _op_div);
        case OP_PRINT:
            return _exec_print(stack);
    }
}

int run_exit(Stack* stack) {
    Data return_code;
    if (stack_pop(stack, &return_code) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Exit] Stack did not end with a value left to return.\n");
        return -1;
    }

    return (int)return_code.as_int64;
}


int _exec_push(Stack* stack, const Data* arg) {
    if (stack_push(stack, arg) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Push] Unable to push the value.\n");
        return -1;
    }
    return 0;
}

int _exec_binary_op(Stack* stack, char* error_id, int (*op)(const Data* first, const Data* second, Data* result)) {
    Data first, second, result;

    if (stack_pop(stack, &first) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [%s] Unable to pop the first operant.\n", error_id);
        return -1;
    }

    if (stack_pop(stack, &second) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [%s] Unable to pop the second operant.\n", error_id);
        return -1;
    }

    int op_status = op(&first, &second, &result);
    if (op_status) return op_status;

    if (stack_push(stack, &result) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [%s] Unable to push the result.\n", error_id);
        return -1;
    }

    return 0;
}

int _exec_print(Stack* stack) {
    Data data;
    if (stack_pop(stack, &data) != STACK_STATUS_OK) {
        fprintf(stderr, "ERROR: [Print] Unable to pop the value to print.\n");
        return -1;
    }

    fprintf(stdout, "%d\n", data.as_int64);
    return 0;
}

int _op_add(const Data* first, const Data* second, Data* result) {
    result->as_int64 = second->as_int64 + first->as_int64;
    return 0;
}

int _op_sub(const Data* first, const Data* second, Data* result) {
    result->as_int64 = second->as_int64 - first->as_int64;
    return 0;
}

int _op_mul(const Data* first, const Data* second, Data* result) {
    result->as_int64 = second->as_int64 * first->as_int64;
    return 0;
}

int _op_div(const Data* first, const Data* second, Data* result) {
    result->as_int64 = second->as_int64 / first->as_int64;
    return 0;
}
