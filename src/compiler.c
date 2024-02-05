#include "compiler.h"

#include <assert.h>

#define IO_BUFFER "io_buffer"
#define IO_BUFFER_SIZE 256

static int32_t _initial_hash();
static void _next_hash(int32_t* hash);

static void _generate_section_bss(FILE* stream);
static void _generate_section_text(const Instructions* inst, FILE* stream, int32_t* hash);

static void _generate_standard_library(FILE* stream);
static void _generate_lib_atoll(FILE* stream);
static void _generate_lib_input(FILE* stream);
static void _generate_lib_print(FILE* stream);
static void _generate_lib_err(FILE* stream);
static void _generate_lib_powll(FILE* stream);

static void _generate_start_symbol(const Instructions* inst, FILE* stream, int32_t* hash);
static void _generate_args(FILE* stream);
static void _generate_instructions(const Instructions* inst, FILE* stream, int32_t* hash);

// Stack manupulation
static void _generate_inst_push(const PushInstructionContext* context, FILE* stream);
static void _generate_inst_pop(FILE* stream);
static void _generate_inst_swap(FILE* stream);
static void _generate_inst_dup(FILE* stream);
static void _generate_inst_deref(FILE* stream);

// IO
static void _generate_inst_print(FILE* stream);
static void _generate_inst_err(FILE* stream);
static void _generate_inst_input(FILE* stream);

// Artithmetic
static void _generate_inst_add(FILE* stream);
static void _generate_inst_sub(FILE* stream);
static void _generate_inst_mul(FILE* stream);
static void _generate_inst_div(FILE* stream);
static void _generate_inst_mod(FILE* stream);
static void _generate_inst_pow(FILE* stream);

// Control flow
static void _generate_inst_if(const IfInstructionContext* context, FILE* stream, int32_t* hash);
static void _generate_inst_while(const WhileInstructionContext* context, FILE* stream, int32_t* hash);
static void _generate_inst_exit(FILE* stream);

int generate_asm(const Instructions* inst, FILE* stream) {
    int32_t hash = _initial_hash();
    _generate_section_bss(stream);
    _generate_section_text(inst, stream, &hash);
    return 0;
}

int32_t _initial_hash() { return 1; }

void _next_hash(int32_t* hash) { ++(*hash); }

void _generate_section_bss(FILE* stream) {
    fprintf(stream,
            "section .bss\n"
            "    %s: resb %d\n"
            "\n"
            "\n",
            IO_BUFFER, IO_BUFFER_SIZE);
}

void _generate_section_text(const Instructions* inst, FILE* stream, int32_t* hash) {
    fprintf(stream,
            "section .text\n"
            "    global _start\n"
            "\n"
            "\n");
    _generate_standard_library(stream);
    _generate_start_symbol(inst, stream, hash);
}

void _generate_standard_library(FILE* stream) {
    _generate_lib_atoll(stream);
    _generate_lib_input(stream);
    _generate_lib_print(stream);
    _generate_lib_err(stream);
    _generate_lib_powll(stream);
}

void _generate_lib_atoll(FILE* stream) {
    fprintf(stream,
            "    atoll:                          ; converts an ascii string (rdi) with length (rsi) to an integer (rax)\n"
            "        ; rax: result       Output\n"
            "        ; rdi: buffer       Input\n"
            "        ; rsi: n            Input\n"
            "        ; rcx: c            Internal\n"
            "        ; rdx: sign         Internal\n"
            "\n"
            "        xor rax, rax                ; zero out results\n"
            "        mov rdx, 1                  ; Assume positive sign if none\n"
            "\n"
            "        cmp rsi, 0                  ; check for empty string -> return with 0\n"
            "        je .end                     ; current state will yield 0\n"
            "\n"
            "        movzx rcx, byte [rdi]       ; load first character\n"
            "        cmp rcx, '+'                ; check for a plus sign\n"
            "        je .plus                    ; consume plus sign before loop\n"
            "        cmp rcx, '-'                ; check for minus sign\n"
            "        je .minus                   ; consume minus sign before loop\n"
            "        \n"
            "        jmp .loop                   ; start loop if no sign was there (+ is assumed)\n"
            "\n"
            "    .plus:\n"
            "        mov rdx, 1                  ; set sign to plus\n"
            "        inc rdi                     ; advance buffer\n"
            "        dec rsi                     ; decrement counter n\n"
            "        jmp .loop                   ; start loop\n"
            "\n"
            "    .minus:\n"
            "        mov rdx, -1                 ; set sign to minus\n"
            "        inc rdi                     ; advance buffer\n"
            "        dec rsi                     ; decrement coutner n\n"
            "        jmp .loop                   ; start loop\n"
            "\n"
            "    .loop:\n"
            "        cmp rsi, 1                  ; check if there are character left\n"
            "        jb .end                     ; if no character are left -> .done\n"
            "\n"
            "        movzx rcx, byte [rdi]       ; load current character\n"
            "        inc rdi                     ; move to next character\n"
            "        dec rsi                     ; reduce counter n\n"
            "\n"
            "        cmp rcx, '0'                ; check for a valid digit character\n"
            "        jb .end\n"
            "        cmp rcx, '9'\n"
            "        ja .end\n"
            "\n"
            "        sub rcx, '0'                ; convert digit character to number\n"
            "        imul rax, 10                ; multiply current result by 10\n"
            "        add rax, rcx                ; add the current number\n"
            "\n"
            "        jmp .loop                   ; continue looping\n"
            "\n"
            "    .end:\n"
            "        imul rax, rdx               ; Apply sign\n"
            "        ret\n"
            "\n"
            "\n");
}

void _generate_lib_input(FILE* stream) {
    fprintf(stream,
            "    input:                          ; reads an integer from the stdin and returns it (rax)\n"
            "        mov rax, 0                  ; read syscall\n"
            "        mov rdi, 0                  ; stdin\n"
            "        mov rsi, %s\n"
            "        mov rdx, %d\n"
            "        syscall                     ; read from stdin into the io buffer\n"
            "\n"
            "        mov rdi, %s\n"
            "        mov rsi, rax                ; only convert the read number of characters\n"
            "        call atoll                  ; convert the string to an integer\n"
            "        ret\n"
            "\n"
            "\n",
            IO_BUFFER, IO_BUFFER_SIZE, IO_BUFFER);
}

void _generate_lib_print(FILE* stream) {
    fprintf(stream,
            "    print:                          ; prints the ascii value (rdi) to stdout\n"
            "        mov [%s], rdi\n"
            "        mov rax, 1                  ; write syscall\n"
            "        mov rdi, 1                  ; stdout\n"
            "        mov rsi, %s\n"
            "        mov rdx, 1                  ; only write one character\n"
            "        syscall                     ; write io buffer to stdout\n"
            "        ret\n"
            "\n"
            "\n",
            IO_BUFFER, IO_BUFFER);
}

void _generate_lib_err(FILE* stream) {
    fprintf(stream,
            "    err:                            ; prints the ascii value (rdi) to stderr\n"
            "        mov [%s], rdi\n"
            "        mov rax, 1                  ; write syscall\n"
            "        mov rdi, 2                  ; stderr\n"
            "        mov rsi, %s\n"
            "        mov rdx, 1                  ; only write one character\n"
            "        syscall                     ; write io buffer to stdout\n"
            "        ret\n"
            "\n"
            "\n",
            IO_BUFFER, IO_BUFFER);
}

void _generate_lib_powll(FILE* stream) {
    fprintf(stream,
            "    powll:              ; computes the exponentiation result (rax) with base (rdi) and exponent (rsi)\n"
            "        ; rax: result   Output\n"
            "        ; rdi: base     Input\n"
            "        ; rsi: exponent Input\n"
            "\n"
            "        mov rax, 1      ; intialize result with 1\n"
            "        cmp rsi, 0      ; check for the sign of the exponent\n"
            "        jae .positive   ; handle zero and positive together\n"
            "\n"
            "    .negative:          ; if exponent is negative\n"
            "        mov rax, 0      ; the result is a fraction with a absolute value less then 1\n"
            "        ret\n"
            "\n"
            "    .positive:\n"
            "        cmp rsi, 0      ; break condition is when exponent is zero\n"
            "        je .done\n"
            "\n"
            "        imul rax, rdi   ; multiply in loop to get the power\n"
            "        dec rsi         ; reduce counter of needed multiplications\n"
            "\n"
            "        jmp .positive   ; continue the loop\n"
            "\n"
            "    .done:\n"
            "        ret\n"
            "\n"
            "\n");
}

void _generate_start_symbol(const Instructions* inst, FILE* stream, int32_t* hash) {
    fprintf(stream, "    _start:\n");
    _generate_args(stream);
    _generate_instructions(inst, stream, hash);
    _generate_inst_exit(stream);
}

void _generate_args(FILE* stream) {
    fprintf(stream,
            "        ; args\n"
            "        ; r12: argc\n"
            "        ; r13: if\n"
            "        ; r14: ptr\n"
            "\n"
            "        mov r12, [rsp]              ; get argc from the stack\n"
            "        dec r12                     ; ignore first argument (program name)\n"
            "        mov r13, 0                  ; i = 0\n"
            "\n"
            "        push 0                      ; place the default return value before args\n"
            "\n"
            "    .args_loop:\n"
            "        cmp r13, r12                ; if (i == argc)\n"
            "        je .args_loop_done          ;     break;\n"
            "\n"
            "        mov r14, r12                ; the current argument to push is argc \n"
            "        add r14, 2                  ; + 2 items above the stack pointer\n"
            "        imul r14, 8                 ; convert count to size\n"
            "        add r14, rsp                ; offset by stack pointer to get pointer to argument\n"
            "\n"
            "        mov rdi, [r14]              ; place argument in rdi for atoll\n"
            "        mov rsi, %d\n"
            "        call atoll\n"
            "\n"
            "        push rax                    ; push the parsed integer argument to the stack\n"
            "        inc r13                     ; ++i\n"
            "\n"
            "        jmp .args_loop              ; continue the loop\n"
            "\n"
            "    .args_loop_done:\n"
            "        push r12                    ; push argc itself on the stack\n"
            "\n",
            IO_BUFFER_SIZE);
}

void _generate_instructions(const Instructions* inst, FILE* stream, int32_t* hash) {
    for (size_t i = 0; i < inst->count; ++i) {
        static_assert(COUNT_INST == 17);  // Exhaustive pattern matching
        switch (inst->items[i].type) {
            // Stack manupulation
            case INST_PUSH:
                _generate_inst_push(&inst->items[i].context.push_instruction, stream);
                break;

            case INST_POP:
                _generate_inst_pop(stream);
                break;

            case INST_SWAP:
                _generate_inst_swap(stream);
                break;

            case INST_DUP:
                _generate_inst_dup(stream);
                break;

            case INST_DEREF:
                _generate_inst_deref(stream);
                break;

            // IO
            case INST_PRINT:
                _generate_inst_print(stream);
                break;

            case INST_ERR:
                _generate_inst_err(stream);
                break;

            case INST_INPUT:
                _generate_inst_input(stream);
                break;

            // Arithmetic
            case INST_ADD:
                _generate_inst_add(stream);
                break;

            case INST_SUB:
                _generate_inst_sub(stream);
                break;

            case INST_MUL:
                _generate_inst_mul(stream);
                break;

            case INST_DIV:
                _generate_inst_div(stream);
                break;

            case INST_MOD:
                _generate_inst_mod(stream);
                break;

            case INST_POW:
                _generate_inst_pow(stream);
                break;

            // Control flow
            case INST_IF:
                _generate_inst_if(&inst->items[i].context.if_instruction, stream, hash);
                break;

            case INST_WHILE:
                _generate_inst_while(&inst->items[i].context.while_instruction, stream, hash);
                break;

            case INST_EXIT:
                _generate_inst_exit(stream);
                break;

            default:
                assert(0 && "Unreachable");
                break;
        }
    }
}

void _generate_inst_push(const PushInstructionContext* context, FILE* stream) {
    fprintf(stream,
            "        ; push %ld\n"
            "        mov rdi, %ld\n"
            "        push rdi\n"
            "\n",
            context->literal, context->literal);
}

void _generate_inst_pop(FILE* stream) {
    fprintf(stream,
            "        ; pop\n"
            "        pop rdi\n"
            "\n");
}

void _generate_inst_swap(FILE* stream) {
    fprintf(stream,
            "        ; swap\n"
            "        mov rdi, [rsp]\n"
            "        mov rsi, [rsp + 8]\n"
            "        mov [rsp], rsi\n"
            "        mov [rsp + 8], rdi\n"
            "\n");
}

void _generate_inst_dup(FILE* stream) {
    fprintf(stream,
            "        ; dup\n"
            "        mov rdi, [rsp]\n"
            "        push rdi\n"
            "\n");
}

void _generate_inst_deref(FILE* stream) {
    fprintf(stream,
            "        ; deref\n"
            "        mov rdi, [rsp]\n"
            "        imul rdi, 8\n"
            "        add rdi, rsp\n"
            "        mov rsi, [rdi]\n"
            "        mov [rsp], rsi\n"
            "\n");
}

void _generate_inst_print(FILE* stream) {
    fprintf(stream,
            "        ; print\n"
            "        pop rdi\n"
            "        call print\n"
            "\n");
}

void _generate_inst_err(FILE* stream) {
    fprintf(stream,
            "        ; err\n"
            "        pop rdi\n"
            "        call err\n"
            "\n");
}

void _generate_inst_input(FILE* stream) {
    fprintf(stream,
            "        ; input\n"
            "        call input\n"
            "        push rax\n"
            "\n");
}

void _generate_inst_add(FILE* stream) {
    fprintf(stream,
            "        ; add\n"
            "        pop rdi\n"
            "        add [rsp], rdi\n"
            "\n");
}

void _generate_inst_sub(FILE* stream) {
    fprintf(stream,
            "        ; sub\n"
            "        pop rdi\n"
            "        sub [rsp], rdi\n"
            "\n");
}

void _generate_inst_mul(FILE* stream) {
    fprintf(stream,
            "        ; mul\n"
            "        pop rdi\n"
            "        mov rsi, [rsp]\n"
            "        imul rsi, rdi\n"
            "        mov [rsp], rsi\n"
            "\n");
}

void _generate_inst_div(FILE* stream) {
    fprintf(stream,
            "        ; div\n"
            "        pop rdi\n"
            "        pop rax\n"
            "        cqo\n"
            "        idiv rdi\n"
            "        push rax\n"
            "\n");
}

void _generate_inst_mod(FILE* stream) {
    fprintf(stream,
            "        ; mod\n"
            "        pop rdi\n"
            "        pop rax\n"
            "        cqo\n"
            "        idiv rdi\n"
            "        push rdx\n"
            "\n");
}

void _generate_inst_pow(FILE* stream) {
    fprintf(stream,
            "        ; pow\n"
            "        pop rsi\n"
            "        pop rdi\n"
            "        call powll\n"
            "        push rax\n"
            "\n");
}

void _generate_inst_if(const IfInstructionContext* context, FILE* stream, int32_t* hash) {
    int32_t this_hash = *hash;
    _next_hash(hash);

    fprintf(stream,
            "        ; if (hash = %d)\n"
            "        pop rdi\n"
            "        cmp rdi, 0\n"
            "        je .else_block_%d\n"
            "\n",
            this_hash, this_hash);

    _generate_instructions(&context->if_block, stream, hash);

    fprintf(stream,
            "        jmp .continue_%d\n"
            "\n"
            "    .else_block_%d:\n"
            "\n",
            this_hash, this_hash);

    _generate_instructions(&context->else_block, stream, hash);

    fprintf(stream,
            "    .continue_%d:\n"
            "\n",
            this_hash);
}

void _generate_inst_while(const WhileInstructionContext* context, FILE* stream, int32_t* hash) {
    int32_t this_hash = *hash;
    _next_hash(hash);

    fprintf(stream,
            "        ; while (hash = %d)\n"
            "    .loop_%d:\n"
            "        pop rdi\n"
            "        cmp rdi, 0\n"
            "        je .continue_%d\n"
            "\n",
            this_hash, this_hash, this_hash);

    _generate_instructions(&context->inner_block, stream, hash);

    fprintf(stream,
            "        jmp .loop_%d\n"
            "\n"
            "    .continue_%d:\n"
            "\n",
            this_hash, this_hash);
}

void _generate_inst_exit(FILE* stream) {
    fprintf(stream,
            "        ; exit\n"
            "        mov rax, 60\n"
            "        pop rdi\n"
            "        syscall\n"
            "\n");
}
