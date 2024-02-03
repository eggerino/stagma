#include "repl.h"

#include <stdio.h>
#include <unistd.h>

#include "dynamic_array.h"
#include "parser.h"
#include "runtime.h"

#define TRUE 1
#define BUFFER_SIZE 256

typedef struct String {
    size_t count;
    size_t capacity;
    char* items;
} String;

static void _print_controls(void);
static char* _read_stdin(void);
static void _print_stack(const Stack* stack);

int repl_main(void) {
    Stack* stack = stack_new(8);

    _print_controls();
    for (size_t input_count = 0; TRUE; ++input_count) {
        // Read
        printf("[%d] >>> ", input_count);
        fflush(stdout);
        fflush(stderr);
        char* input = _read_stdin();
        if (input == NULL) {
            break;
        }
        Instructions* instructions = parse_instructions(input);
        free(input);

        // Evaluate
        stack_execute(stack, instructions);

        instructions_delete(instructions);

        // Print
        _print_stack(stack);
    }

    int return_code = 0;
    if (stack->count > 0) {
        return_code = stack_top(*stack);
    }

    stack_delete(stack);

    return return_code;
}

void _print_controls(void) {
    printf("stagma REPL\n");
    printf("\tCTRL+D:\tconfirm input\n");
    printf("\tCTRL+C:\texit\n");
}

char* _read_stdin(void) {
    String str = {0};

    // Keep reading everything from stdin to str
    char buffer[BUFFER_SIZE];
    ssize_t num_read;
    while ((num_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        da_extend(str, buffer, num_read);
    }

    if (num_read == -1) {       // Return null on error (e.g. interupt signal)
        free(str.items);
        return NULL;
    }

    da_push(str, '\0');
    return str.items;    
}

void _print_stack(const Stack* stack) {
    printf("\n\n");

    if (stack->count == 0) {
        printf("-- empty stack --\n");
    }

    for (size_t i = 0; i < stack->count; ++i) {
        printf("[%d]:\t%d\n", i, stack->items[i]);
    }
    printf("\n");
}
