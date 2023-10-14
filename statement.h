#ifndef STATEMENT_H_
#define STATEMENT_H_

#include <stdint.h>

typedef enum Operation {
    OP_PUSH,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_PRINT,
    OP_COUNT,
} Operation;

typedef union Data {
    int64_t as_int64;
} Data;

typedef struct Statement {
    Operation op;
    Data arg;
} Statement;

#endif
