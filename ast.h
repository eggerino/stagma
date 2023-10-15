#ifndef AST_H_
#define AST_H_

#include <stdint.h>

typedef enum Operation {
    OP_PUSH,
    OP_POP,

    OP_DUB,
    OP_SWAP,

    OP_JUMP,

    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,

    OP_PRINT,
    OP_DEBUG,

    OP_COUNT,
} Operation;

typedef struct Statement {
    Operation op;
    int64_t arg;
} Statement;

#endif
