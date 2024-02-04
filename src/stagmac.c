#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "info.h"
#include "io.h"
#include "parser.h"

static int _help(const char* program_name);

int main(int argc, const char** argv) {
    const char* program_name = argv[0];

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            return _help(program_name);
        }

        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            print_version(program_name);
            return 0;
        }
    }

    assert(argc == 4 && "Invalid number of arguments given");
    assert((strcmp(argv[1], "-o") == 0 || strcmp(argv[1], "--output") == 0) && "Second argument must be -o ir --output");

    char* content = io_read_entire_file(argv[3]);
    Instructions* instructions = parse_instructions(content);
    free(content);

    FILE* output_file = fopen(argv[2], "w");
    int result = generate_asm(instructions, output_file);
    instructions_delete(instructions);
    fclose(output_file);

    return result;
}

int _help(const char* program_name) {
    printf("Usage:\n");
    printf("\t%s -o | --output <OUTPUT-FILE> <SOURCE-FILE>  generate x64 assembly for nasm\n", program_name);
    printf("\t%s -h | --help                                show help\n", program_name);
    printf("\t%s -v | --version                             show version\n", program_name);
    return 0;
}
