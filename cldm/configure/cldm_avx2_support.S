    .section .data
    avx2_y:          .ascii "avx2_support := y\n"
    avx2_y_len =     . - avx2_y
    avx2_n:          .ascii "avx2_support := n\n"
    avx2_n_len =     . - avx2_n
    no_file:         .ascii "No filename supplied\n"
    no_file_len =    . - no_file
    open_fail:       .ascii "Open syscall failed\n"
    open_fail_len =  . - open_fail
    write_fail:      .ascii "Write syscall failed\n"
    write_fail_len = . - write_fail

    .section .text

    .equ SC_WRITE, 1
    .equ SC_OPEN,  2
    .equ SC_CLOSE, 3
    .equ SC_EXIT,  60
    .equ O_WRONLY, 0001
    .equ O_CREAT,  0100
    .equ STDERR_FILENO, 2

.macro exiterr str, len
    movl    $STDERR_FILENO, %edi                    # Write to stderr
    movl    $SC_WRITE, %eax                         # Syscall write
    syscall

    movl    $0x01, %edi                             # Exit status
    movl    $SC_EXIT, %eax                          # Exit syscall
    syscall
.endm

.globl main
main:
    cmpl    $0x01, %edi                             # Check for output file
    jng     .Lno_filearg

    movq    %rbx, %r9                               # Preserve rbx

    movl    $0x01, %eax                             # Request version information
    cpuid

    movq    %r9, %rbx                               # Restore rbx

    andl    $0xc000000, %ecx
    cmpl    $0xc000000, %ecx                        # Check osxsave and avx feature flags
    jne     .Lno_avx2

    xorl    %ecx, %ecx
    xgetbv                                          # Read extended control register 0

    andl    $0x03, %eax
    cmpl    $0x03, %eax                             # Check xmm and ymm state support
    jne     .Lno_avx2

    leaq    avx2_y(%rip), %r8                       # Load avx2 string
    movl    $avx2_y_len, %r9d

    jmp     .Lwrite

.Lno_avx2:
    leaq    avx2_n(%rip), %r8                       # Load string
    movl    $avx2_n_len, %r9d                       # Load length

.Lwrite:
    movl    $SC_OPEN, %eax                          # Open syscall
    movq    0x08(%rsi), %rdi
    movl    $O_CREAT | O_WRONLY, %esi               # Flags
    movl    $0644, %edx                             # Mode
    syscall

    cmpl    $0x00, %eax                             # Check for syscall failure
    jl      .Lopen_failure

    movq    %r8, %rsi                               # String to write
    movl    %r9d, %edx                              # Length
    movl    %eax, %edi                              # File descriptor
    movl    $SC_WRITE, %eax                         # Syscall write
    syscall

    cmpl    $0x00, %eax                             # Check syscall failure
    jl      .Lwrite_failure

    movl    $SC_CLOSE, %eax                         # Close syscall
    syscall

    xorl    %edi, %edi                              # Exit status
    movl    $SC_EXIT, %eax                          # Exit syscall
    syscall

.Lno_filearg:
    leaq    no_file(%rip), %rsi                     # Load string and length
    movq    $no_file_len, %rdx
    exiterr

.Lopen_failure:
    leaq    open_fail(%rip), %rsi                   # Load string and length
    movq    $open_fail_len, %rdx
    exiterr

.Lwrite_failure:
    movl    $SC_CLOSE, %eax                         # Close syscall
    syscall
    leaq    write_fail(%rip), %rsi                  # Load string and length
    movq    $write_fail_len, %rdx
    exiterr
