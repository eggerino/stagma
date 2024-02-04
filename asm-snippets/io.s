;
; x64 asm for the io instructions
;


section .bss
    ioBuffer: resb 256          ; 256 byte buffer for io


section .text
    global _start


    atoll:                          ; converts an ascii string (rdi) with length (rsi) to an integer (rax)
        ; rax: result       Output
        ; rdi: buffer       Input
        ; rsi: n            Input
        ; rcx: c            Internal
        ; rdx: sign         Internal

        xor rax, rax                ; zero out results
        mov rdx, 1                  ; Assume positive sign if none

        cmp rsi, 0                  ; check for empty string -> return with 0
        je .end                     ; current state will yield 0

        movzx rcx, byte [rdi]       ; load first character
        cmp rcx, '+'                ; check for a plus sign
        je .plus                    ; consume plus sign before loop
        cmp rcx, '-'                ; check for minus sign
        je .minus                   ; consume minus sign before loop
        
        jmp .loop                   ; start loop if no sign was there (+ is assumed)

    .plus:
        mov rdx, 1                  ; set sign to plus
        inc rdi                     ; advance buffer
        dec rsi                     ; decrement counter n
        jmp .loop                   ; start loop

    .minus:
        mov rdx, -1                 ; set sign to minus
        inc rdi                     ; advance buffer
        dec rsi                     ; decrement coutner n
        jmp .loop                   ; start loop

    .loop:
        cmp rsi, 1                  ; check if there are character left
        jb .end                     ; if no character are left -> .done

        movzx rcx, byte [rdi]       ; load current character
        inc rdi                     ; move to next character
        dec rsi                     ; reduce counter n

        cmp rcx, '0'                ; check for a valid digit character
        jb .end
        cmp rcx, '9'
        ja .end

        sub rcx, '0'                ; convert digit character to number
        imul rax, 10                ; multiply current result by 10
        add rax, rcx                ; add the current number

        jmp .loop                   ; continue looping

    .end:
        imul rax, rdx               ; Apply sign
        ret


    input:                          ; reads an integer from the stdin and returns it (rax)
        mov rax, 0                  ; read syscall
        mov rdi, 0                  ; stdin
        mov rsi, ioBuffer           ; use ioBuffer
        mov rdx, 256                ; use all 256 bytes
        syscall                     ; read from stdin into the io buffer

        mov rdi, ioBuffer           ; use ioBuffer
        mov rsi, rax                ; only convert the read number of characters
        call atoll                  ; convert the string to an integer
        ret


    print:                          ; prints the ascii value (rdi) to stdout
        mov [ioBuffer], rdi         ; place the value to print into ioBuffer
        mov rax, 1                  ; write syscall
        mov rdi, 1                  ; stdout
        mov rsi, ioBuffer           ; use ioBuffer
        mov rdx, 1                  ; only write one character
        syscall                     ; write io buffer to stdout
        ret


    err:                            ; prints the ascii value (rdi) to stderr
        mov [ioBuffer], rdi         ; place the value to print into ioBuffer
        mov rax, 1                  ; write syscall
        mov rdi, 2                  ; stderr
        mov rsi, ioBuffer           ; use ioBuffer
        mov rdx, 1                  ; only write one character
        syscall                     ; write io buffer to stdout
        ret


    _start:

        ; args
        ; r12: argc
        ; r13: if
        ; r14: ptr

        mov r12, [rsp]              ; get argc from the stack
        dec r12                     ; ignore first argument (program name)
        mov r13, 0                  ; i = 0

    .args_loop:
        cmp r13, r12                ; if (i == argc)
        je .args_loop_done          ;     break;

        mov r14, r12                ; the current argument to push is argc 
        inc r14                     ; + 1 items above the stack pointer
        imul r14, 8                 ; convert count to size
        add r14, rsp                ; offset by stack pointer to get pointer to argument

        mov rdi, [r14]              ; place argument in rdi for atoll
        mov rsi, 256                ; use null termination for stop criteria in atoll
        call atoll

        push rax                    ; push the parsed integer argument to the stack
        inc r13                     ; ++i

        jmp .args_loop              ; continue the loop

    .args_loop_done:
        push r12                    ; push argc itself on the stack

        ; actual program starts here
        
        ; pop
        pop rdi
    
        ; print
        pop rdi
        call print

        ; print
        pop rdi
        call print

        ; print
        pop rdi
        call print

        ; input
        call input
        push rax

        ; print
        pop rdi
        call print

        ; input
        call input
        push rax

        ; err
        pop rdi
        call err

        mov rax, 60     ; exit syscall
        mov rdi, 0      ; Return with 0
        syscall
