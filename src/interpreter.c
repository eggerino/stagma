#include "interpreter.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "dynamic_array.h"
#include "parser.h"
#include "runtime.h"

static char* _read_entire_file(const char* file);
static void _push_arg(Stack* stack, const char* arg);

int interpreter_main(const char* file, int argc, const char** argv) {
    char* content = _read_entire_file(file);
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

char* _read_entire_file(const char* file) {
    FILE* f = fopen(file, "r");
    assert(f && "Could not open file");

    // Get content size
    fseek(f, 0L, SEEK_END);
    long content_size = ftell(f);
    rewind(f);

    // Create buffer
    long buffer_size = content_size + 1;
    char* buffer = (char*)malloc(buffer_size);
    assert(buffer && "Could not create enough space to read file");

    // Read into buffer as a c string (null terminated)
    fread(buffer, content_size, 1, f);
    buffer[buffer_size - 1] = '\0';

    fclose(f);

    return buffer;
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
