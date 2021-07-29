    .section .text

    #include <asm/unistd.h>

.equ    AT_FDCWD,     -0x64
.equ    O_WRONLY,      0x01
.equ    O_CREAT,       0100
.equ    O_TRUNC,       01000
.equ    S_RDWR,        0666
.equ    STDERR_FILENO, 0x02

.macro  exiterr, file, length
    adr     x1, \file
    adr     x2, \length
    ldr     w2, [x2]
    mov     x0, #STDERR_FILENO                      // Write to stderr
    mov     x8, #__NR_write                         // write syscall
    svc     0

    mov     x0, #0x01                               // Exit status
    mov     x8, #__NR_exit
    svc     0
.endm

.globl  main
main:
    cmp     w0, #0x01                               // Check for output file
    b.le    .Lno_filearg

    mrs     x2, ID_AA64PFR0_EL1                     // Read processor feature register 0

    tbnz    x2, #0x17, .Lnone                       // If bit 23 is set, no support
    tbnz    x2, #0x14, .Lfull                       // If bit 20 is set, full support, including half precision

    adr     x6, advsimd_p                           // Partial support, no half precision
    adr     x7, advsimd_p_len
    ldr     w7, [x7]

    b       .Lwrite

.Lfull:
    adr     x6, advsimd_y                           // Full support
    adr     x7, advsimd_y_len
    ldr     w7, [x7]

    b       .Lwrite

.Lnone:
    adr     x6, advsimd_n                           // No support
    adr     x7, advsimd_n_len
    ldr     w7, [x7]

.Lwrite:
    mov     x5, #0x01                               // Index of file name in argv
    mov     x0, #AT_FDCWD                           // Open relative current dir
    ldr     x1, [x1, x5, lsl #0x03]                 // Address of file name
    mov     x2, #O_WRONLY | O_CREAT | O_TRUNC       // Flags
    mov     x3, #S_RDWR                             // Permissions
    mov     x8, #__NR_openat                        // openat syscall
    svc     0

    cmp     x0, 0x00                                // Check return value
    b.lt    .Lopen_failure

    mov     x1, x6                                  // String to write
    mov     x2, x7                                  // Length of string
    mov     x8, #__NR_write                         // write syscall
    svc     0

    cmp     x0, 0x00                                // Check return value
    b.lt    .Lwrite_failure

    mov     x8, #__NR_fsync                         // Sync to file
    svc     0

    mov     x8, #__NR_close                         // Close syscall
    svc     0

    eor     x0, x0, x0                              // Return value
    mov     x8, __NR_exit                           // Exit syscall
    svc     0

.Lno_filearg:
    exiterr no_file, no_file_len

.Lopen_failure:
    exiterr open_fail, open_fail_len

.Lwrite_failure:
    mov     x8, #__NR_close                         // Close file
    svc     0
    exiterr write_fail, write_fail_len

    .section .data
    advsimd_y:      .ascii "advsimd_support := y\n"
    advsimd_y_len:  .word . - advsimd_y
    advsimd_p:      .ascii "advsimd_support := p\n"
    advsimd_p_len:  .word . - advsimd_p
    advsimd_n:      .ascii "advsimd_support := n\n"
    advsimd_n_len:  .word . - advsimd_n
    no_file:        .ascii "No filename supplied\n"
    no_file_len:    .word . - no_file
    open_fail:      .ascii "Open syscall failed\n"
    open_fail_len:  .word . - open_fail
    write_fail:     .ascii "Write syscall failed\n"
    write_fail_len: .word . - write_fail
