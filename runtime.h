#ifndef RUNTIME_H_
#define RUNTIME_H_

#include <stddef.h>

#include "stack.h"
#include "statement.h"

int run_start(Stack* stack, int argc, char** argv);
int run_statement(Stack* stack, Statement* statement);
int run_exit(Stack* stack);

#endif
