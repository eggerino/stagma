;
; x64 asm for the stack manipulation instructions
;


section .text
    global _start


    _start:

    .push:
        ; push 1
        mov rdi, 1
        push rdi

        pop rdi
        mov rax, 60
        ; syscall

    .pop:
        ; push 2
        mov rdi, 2
        push rdi

        ; push 3
        mov rdi, 3
        push rdi

        ; pop
        pop rdi

        pop rdi
        mov rax, 60
        ; syscall
    
    .swap:
        ; push 4
        mov rdi, 4
        push rdi

        ; push 5
        mov rdi, 5
        push rdi

        ; swap
        mov rdi, [rsp]
        mov rsi, [rsp + 8]
        mov [rsp], rsi
        mov [rsp + 8], rdi

        pop rdi
        mov rax, 60
        ; syscall

    .dup:
        ; push 6
        mov rdi, 6
        push rdi

        ; dup
        mov rdi, [rsp]
        push rdi
        
        pop rdi
        mov rax, 60
        ; syscall

    .deref:
        ; push 7
        mov rdi, 7
        push rdi

        ; push 8
        mov rdi, 8
        push rdi

        ; push 2
        mov rdi, 2
        push rdi

        ; deref
        mov rdi, [rsp]
        imul rdi, 8
        add rdi, rsp
        mov rsi, [rdi]
        mov [rsp], rsi

        pop rdi
        mov rax, 60
        syscall