    section .data
    avx2_y:         db "avx2_support := y",0xa
    avx2_y_len:     equ $-avx2_y
    avx2_n:         db "avx2_support := n",0xa
    avx2_n_len:     equ $-avx2_n
    no_file:        db "No filename supplied",0xa
    no_file_len:    equ $-no_file
    open_fail:      db "Open syscall failed",0xa
    open_fail_len:  equ $-open_fail
    write_fail:     db "Write syscall failed",0xa
    write_fail_len: equ $-write_fail

    section .text

    default rel

    global _start

    %define SC_WRITE 1
    %define SC_OPEN  2
    %define SC_CLOSE 3
    %define SC_EXIT 60
    %define O_CREAT 0102o
    %define STDERR_FILENO 2

    %macro exit_err 2
        %ifdef CLOSE_FILE
            mov     eax, SC_CLOSE           ; Close syscall
            syscall
        %endif

        lea     rsi, [%1]                   ; Load string
        mov     rdx, %2                     ; Load length

        mov     edi, STDERR_FILENO          ; Write to stderr
        mov     eax, SC_WRITE               ; Syscall write
        syscall

        mov     edi, 1                      ; Exit status
        mov     eax, SC_EXIT                ; Exit syscall
        syscall
    %endmacro

_start:
.argc       equ 0
.argv1      equ 0x10

    cmp     dword [rsp + .argc], 1          ; Check for output file
    jng     .no_filearg

    mov     r9, rbx                         ; Preserve rbx

    mov     eax, 1                          ; Request version information
    cpuid

    mov     rbx, r9                         ; Restore rbx

    and     ecx, 0xC000000
    cmp     ecx, 0xC000000                  ; Check osxsave and avx feature flags
    jne     .no_avx2

    xor     ecx, ecx
    xgetbv                                  ; Read extended control register 0

    and     eax, 0x3
    cmp     eax, 0x3                        ; Check xmm and ymm state support
    jne     .no_avx2

    lea     r8, [avx2_y]                    ; Load avx2 string
    mov     r9d, avx2_y_len                 ; Load length

    jmp     .write

.no_avx2:
    lea     r8, [avx2_n]                    ; Load string
    mov     r9d, avx2_n_len                 ; Load length

.write:
    mov     eax, SC_OPEN                    ; Open syscall
    mov     rdi, [rsp + .argv1]             ; Filename to rdi
    mov     esi, O_CREAT                    ; Flags
    mov     rdx, 0664o                      ; Mode
    syscall

    cmp     rax, 0                          ; Check for syscall failure
    jl      .open_failure

    mov     rsi, r8                         ; String to write
    mov     edx, r9d                        ; Length
    mov     edi, eax                        ; File descriptor
    mov     eax, SC_WRITE                   ; Write syscall
    syscall

    cmp     rax, 0                          ; Check syscall failure
    jl      .write_failure

    mov     eax, SC_CLOSE                   ; Close syscall
    syscall

    xor     edi, edi                        ; Exit status
    mov     eax, SC_EXIT                    ; Exit syscall
    syscall

.no_filearg:
    exit_err    no_file, no_file_len

.open_failure:
    exit_err    open_fail, open_fail_len

.write_failure:
%define CLOSE_FILE
    exit_err    write_fail, write_fail_len
%undef CLOSE_FILE
