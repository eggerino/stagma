#include "interpreter.h"
#include "repl.h"

int main(int argc, const char** argv) {
    if (argc > 1) {
        return interpreter_main(argv[1], argc - 2, &argv[2]);
    } else {
        return repl_main();
    }
}
