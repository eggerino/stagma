#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "runtime.h"
#include "stack.h"

#define INITIAL_STACK_CAPACITY 10

int read_byte_code(const char* filename, Statement** program, int64_t* num_statements) {
    FILE* f = fopen(filename, "rb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: [Runtime] Cannot open byte code file,\n");
        return -1;
    }

    fread(num_statements, sizeof(*num_statements), 1, f);

    *program = (Statement*)malloc(sizeof(**program) * (*num_statements));
    fread(*program, sizeof(**program), *num_statements, f);

    fclose(f);
    return 0;
}

int run_runtime(Stack* stack, Statement* program, int64_t num_statements, int argc, char** argv) {
    int status;
    status = run_start(stack, argc, argv);
    if (status) return status;
    
    for (int64_t i = 0; i < num_statements; ++i) {
        status = run_statement(stack, &program[i], &i);
        if (status) return status;
    }

    return run_exit(stack);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "ERROR: [Runtime] No filename of the byte code is specified.\n");
        return -1;
    }

    int status;
    Statement* program;
    int64_t num_statements;
    status = read_byte_code(argv[1], &program, &num_statements);
    if (!status) {
        Stack* stack = stack_new(INITIAL_STACK_CAPACITY);
        if (stack == NULL) {
            fprintf(stderr, "ERROR: [Runtime] Unable to initialize stack.\n");
            status = -1;
        } else {
            status = run_runtime(stack, program, num_statements, argc - 1, argv + 1);
            stack_delete(stack);
        }

        free(program);
    }

    return status;
}
