;
; x64 asm for the control flow instructions
;


section .text
    global _start


    exit:                           ; uses a given error code (rdi) and exits the program
        mov rax, 60                 ; exit syscall
        syscall                     ; return with code (rdi)
        ret


    _start:

    .while:
        ; push 1
        mov rdi, 1
        push rdi

        ; while (hash = 123)
    .loop_123:
        pop rdi
        cmp rdi, 0
        je .continue_123

        ; the code in the inner block
        mov rdi, 0
        push rdi
        ; end code in the inner block    

        jmp .loop_123

    .continue_123:

        ; Next instructions

    .if:
        ; push 1
        mov rdi, 1
        push rdi

        ; if (hash = 1234)
        pop rdi
        cmp rdi, 0
        je .else_block_1234

        ; the code in the if block
        mov rdi, 1
        call exit
        ; end code in the if block

        jmp .continue_1234

    .else_block_1234:

        ; the code in the else block
        mov rdi, 2
        call exit
        ; end code in the else block

    .continue_1234:

        ; Next instructions

        ; exit
        pop rdi
        call exit
