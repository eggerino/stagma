#include "info.h"

#include <stdio.h>

void print_version(const char* program_name) {
    printf("%s version %s\n", program_name, VERSION);
}
