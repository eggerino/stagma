#ifndef AST_H_
#define AST_H_

#include <stddef.h>
#include <stdint.h>

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
    INST_POP,
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

    // Control flow
    INST_IF,
    INST_WHILE,
    INST_EXIT,

    // Exhaustive assert value
    COUNT_INST
} InstructionType;

typedef struct PushInstructionContext {
    int64_t literal;
} PushInstructionContext;

typedef struct IfInstructionContext {
    Instructions if_block;
    Instructions else_block;
} IfInstructionContext;

typedef struct WhileInstructionContext {
    Instructions inner_block;
} WhileInstructionContext;

#define COUNT_INSTRUCTIONCONTEXT 3
typedef union InstructionContext {
    PushInstructionContext push_instruction;
    IfInstructionContext if_instruction;
    WhileInstructionContext while_instruction;
} InstructionContext;

struct Instruction {
    InstructionType type;
    InstructionContext context;
};

void instructions_delete(Instructions* inst);

#endif
