#include <stdio.h>

#include "interpreter.h"

int main(int argc, const char** argv) {
    if (argc > 1) {
        return interpreter_main(argv[1], argc - 2, &argv[2]);
    } else {
        fprintf(stderr, "REPL Not implemented yet\n");
        return 1;
    }
}
