#include <stdio.h>
#include "runtime.h"
#include "stack.h"
#include "statement.h"

#define INITIAL_STACK_CAPACITY 10

Statement PROGRAM[] = {
    { .op = OP_ADD },
    { .op = OP_PRINT },
    { .arg = {.as_int64 = 1400}, .op = OP_PUSH},
    { .arg = {.as_int64 = 63}, .op = OP_PUSH},
    { .op = OP_SUB },
    { .op = OP_PRINT },
    { .arg = {.as_int64 = 10}, .op = OP_PUSH},
    { .arg = {.as_int64 = 42}, .op = OP_PUSH},
    { .op = OP_MUL},
    { .op = OP_PRINT },
    { .arg = {.as_int64 = 210}, .op = OP_PUSH},
    { .arg = {.as_int64 = 5}, .op = OP_PUSH},
    { .op = OP_DIV},
    { .op = OP_PRINT },
    { .arg = {.as_int64 = 0}, .op = OP_PUSH},
};

int run_runtime(Stack* stack, Statement* program, size_t num_statements, int argc, char** argv) {
    int status;
    status = run_start(stack, argc, argv);
    if (status) return status;
    
    for (size_t i = 0; i < num_statements; ++i) {
        status = run_statement(stack, &program[i]);
        if (status) return status;
    }

    return run_exit(stack);
}

int main(int argc, char** argv) {
    Stack* stack = stack_new(INITIAL_STACK_CAPACITY);
    if (stack == NULL) {
        fprintf(stderr, "ERROR: [Runtime] Unable to initialize stack.\n");
        return -1;
    }

    int status = run_runtime(stack, PROGRAM, sizeof(PROGRAM) / sizeof(PROGRAM[0]), argc, argv);
    stack_delete(stack);
    return status;
}
