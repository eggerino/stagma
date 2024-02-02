#ifndef AST_H_
#define AST_H_

#include <stddef.h>

// Forward declare instruction so a pointer can be build before defintion
typedef struct Instruction Instruction;

typedef struct Instructions {
    size_t count;
    size_t capacity;
    Instruction* items;
} Instructions;

typedef enum InstructionType {
    // Stack manipulation
    INST_PUSH,
    INST_PINST,
    INST_SWAP,
    INST_DUP,
    INST_DEREF,

    // IO
    INST_PRINT,
    INST_ERR,
    INST_INPUT,

    // Arithmetic
    INST_ADD,
    INST_SUB,
    INST_MUL,
    INST_DIV,
    INST_MOD,
    INST_POW,

    // Conditionals
    INST_IF,
    INST_WHILE,

    // Exhaustive assert value
    COUNT_INST
} InstructionType;

typedef struct IfInstructionContext {
    Instructions inner_block;
    Instructions else_block;
} IfInstructionContext;

typedef struct WhileInstructionContext {
    Instructions inner_block;
} WhileInstructionContext;

typedef union InstructionContext {
    IfInstructionContext if_instruction;
    WhileInstructionContext while_instruction;
} InstructionContext;

struct Instruction {
    InstructionType type;
    InstructionContext context;
};

void instructions_delete(Instructions* inst);

#endif
