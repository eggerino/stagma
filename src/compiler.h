#ifndef COMPILER_H_
#define COMPILER_H_

#include <stdio.h>

#include "ast.h"

int generate_asm(const Instructions* inst, FILE* stream);

#endif
