#include "ast.h"

#include <assert.h>
#include <stdlib.h>

static void _free_fields(Instructions* inst);

void instructions_delete(Instructions* inst) {
    _free_fields(inst);
    free(inst);
}

void _free_fields(Instructions* inst) {
    for (size_t i = 0; i < inst->count; ++i) {
        static_assert(COUNT_INSTRUCTIONCONTEXT == 2);
        switch (inst->items[i].type) {
            case INST_IF:
                _free_fields(&inst->items[i].context.if_instruction.inner_block);
                _free_fields(&inst->items[i].context.if_instruction.else_block);
                break;

            case INST_WHILE:
                _free_fields(&inst->items[i].context.while_instruction.inner_block);
                break;

            default:
                break;
        }
    }

    free(inst->items);
}
