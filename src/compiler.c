#include "compiler.h"

#define IO_BUFFER "io_buffer"
#define IO_BUFFER_SIZE 256

static int32_t _initial_hash();
static void _next_hash(int32_t* hash);

static void _generate_section_bss(FILE* stream);
static void _generate_section_text(const Instructions* inst, FILE* stream, int32_t* hash);
static void _generate_standard_library(FILE* stream);
static void _generate_atoll(FILE* stream);
static void _generate_input(FILE* stream);
static void _generate_print(FILE* stream);
static void _generate_err(FILE* stream);
static void _generate_powll(FILE* stream);
static void _generate_start_symbol(const Instructions* inst, FILE* stream, int32_t* hash);
static void _generate_args(FILE* stream);

static void _generate_exit(FILE* stream);

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
    _generate_atoll(stream);
    _generate_input(stream);
    _generate_print(stream);
    _generate_err(stream);
    _generate_powll(stream);
}

void _generate_atoll(FILE* stream) {
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

void _generate_input(FILE* stream) {
    fprintf(stream,
            "    input:                          ; reads an integer from the stdin and returns it (rax)\n"
            "        mov rax, 0                  ; read syscall\n"
            "        mov rdi, 0                  ; stdin\n"
            "        mov rsi, %s\n"
            "        mov rdx, 256                ; use all 256 bytes\n"
            "        syscall                     ; read from stdin into the io buffer\n"
            "\n"
            "        mov rdi, %s\n"
            "        mov rsi, rax                ; only convert the read number of characters\n"
            "        call atoll                  ; convert the string to an integer\n"
            "        ret\n"
            "\n"
            "\n",
            IO_BUFFER, IO_BUFFER);
}

void _generate_print(FILE* stream) {
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

void _generate_err(FILE* stream) {
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

void _generate_powll(FILE* stream) {
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

    _generate_exit(stream);
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
            "        mov rsi, 256                ; use null termination for stop criteria in atoll\n"
            "        call atoll\n"
            "\n"
            "        push rax                    ; push the parsed integer argument to the stack\n"
            "        inc r13                     ; ++i\n"
            "\n"
            "        jmp .args_loop              ; continue the loop\n"
            "\n"
            "    .args_loop_done:\n"
            "        push r12                    ; push argc itself on the stack\n"
            "\n");
}

void _generate_exit(FILE* stream) {
    fprintf(stream,
            "        ; exit\n"
            "        mov rax, 60\n"
            "        pop rdi\n"
            "        syscall\n");
}
