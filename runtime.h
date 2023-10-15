#ifndef RUNTIME_H_
#define RUNTIME_H_

#include <stddef.h>

#include "ast.h"
#include "stack.h"

int run_start(Stack* stack, int argc, char** argv);
int run_statement(Stack* stack, Statement* statement, int64_t* current_statement);
int run_exit(Stack* stack);

#endif
