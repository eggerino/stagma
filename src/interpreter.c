#include "interpreter.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "dynamic_array.h"
#include "io.h"
#include "parser.h"
#include "runtime.h"

static void _push_arg(Stack* stack, const char* arg);

int interpreter_main(const char* file, int argc, const char** argv) {
    char* content = io_read_entire_file(file);
    Instructions* instructions = parse_instructions(content);
    free(content);

    Stack* stack = stack_new(argc + 1);

    for (int i = argc - 1; i >= 0; --i) {
        _push_arg(stack, argv[i]);
    }
    da_push(*stack, argc);

    stack_execute(stack, instructions);

    instructions_delete(instructions);

    int return_code = 0;
    if (stack->count > 0) {
        return_code = stack_top(*stack);
    }

    stack_delete(stack);

    return return_code;
}

void _push_arg(Stack* stack, const char* arg) {
    int64_t sign = 1;
    if (*arg == '-') {
        sign = -1;
        ++arg;
    }

    assert(isdigit(*arg) && "Only integers can be used as args");
    int64_t value = sign * atoll(arg);

    da_push(*stack, value);
}
