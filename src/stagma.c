#include <stdio.h>
#include <string.h>

#include "info.h"
#include "interpreter.h"
#include "repl.h"

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

    if (argc > 1) {
        return interpreter_main(argv[1], argc - 2, &argv[2]);
    } else {
        return repl_main(program_name);
    }
}

int _help(const char* program_name) {
    printf("Usage:\n");
    printf("\t%s                           start repl\n", program_name);
    printf("\t%s <SOURCE-FILE> [...ARGS]   interpret Source-FILE with the given ARGS\n", program_name);
    printf("\t%s -h | --help               show help\n", program_name);
    printf("\t%s -v | --version            show version\n", program_name);
    return 0;
}
