;
; x64 asm for the arithmetic instructions
;


section .text
    global _start


    powll:              ; computes the exponentiation result (rax) with base (rdi) and exponent (rsi)
        ; rax: result   Output
        ; rdi: base     Input
        ; rsi: exponent Input

        mov rax, 1      ; intialize result with 1
        cmp rsi, 0      ; check for the sign of the exponent
        jae .positive   ; handle zero and positive together

    .negative:          ; if exponent is negative
        mov rax, 0      ; the result is a fraction with a absolute value less then 1
        ret

    .positive:
        cmp rsi, 0      ; break condition is when exponent is zero
        je .done

        imul rax, rdi   ; multiply in loop to get the power
        dec rsi         ; reduce counter of needed multiplications

        jmp .positive   ; continue the loop

    .done:
        ret


    _start:

    .add:
        ; push 1
        mov rdi, 1
        push rdi

        ; push 2
        mov rdi, 2
        push rdi

        ; add
        pop rdi
        add [rsp], rdi

        pop rdi
        mov rax, 60
        ; syscall

    .sub:
        ; push 5
        mov rdi, 5
        push rdi

        ; push 1
        mov rdi, 1
        push rdi

        ; sub
        pop rdi
        sub [rsp], rdi

        pop rdi
        mov rax, 60
        ; syscall

    .mul:
        ; push 5
        mov rdi, 5
        push rdi

        ; push 2
        mov rdi, 2
        push rdi

        ; mul
        pop rdi
        mov rsi, [rsp]
        imul rsi, rdi
        mov [rsp], rsi

        pop rdi
        mov rax, 60
        ; syscall

    .div:
        ; push 16
        mov rdi, 16
        push rdi

        ; push 3
        mov rdi, 3
        push rdi

        ; div
        pop rdi
        pop rax
        cqo
        idiv rdi
        push rax

        pop rdi
        mov rax, 60
        ; syscall

    .mod:
        ; push 17
        mov rdi, 17
        push rdi

        ; push 3
        mov rdi, 3
        push rdi

        ; div
        pop rdi
        pop rax
        cqo
        idiv rdi
        push rdx

        pop rdi
        mov rax, 60
        ; syscall

    .pow:
        ; push 9
        mov rdi, 9
        push rdi

        ; push 2
        mov rdi, 2
        push rdi

        ; pow
        pop rsi
        pop rdi
        call powll
        push rax

        pop rdi
        mov rax, 60
        syscall