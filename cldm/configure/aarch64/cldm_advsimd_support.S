    .section .text

    #include <asm/unistd.h>

.equ    AT_FDCWD,     -0x64
.equ    O_WRONLY,      0x01
.equ    O_CREAT,       0100
.equ    O_TRUNC,       01000
.equ    S_RDWR,        0666
.equ    STDERR_FILENO, 0x02

.macro  exit, status=0
    mov     x0, \status
    mov     x8, #__NR_exit
    svc     0
.endm

.macro  exiterr, msg
    adr     x1, \msg
    adr     x2, \msg\()_len
    ldr     w2, [x2]
    mov     x0, #STDERR_FILENO                                      // Write to stderr
    mov     x8, #__NR_write                                         // write syscall
    svc     0

    exit    1
.endm

.macro  strtup, name, val
    \name\()_\val\():       .ascii "\name := \val\n"
    \name\()_\val\()_len:   .word . - \name\()_\()\val
.endm

.macro strvtup, name, val
    \name\():               .ascii "\val"
    \name\()_len:           .word . - \name
.endm

.macro  write, string
    mov     w0, w15
    adr     x1, \string
    adr     x4, \string\()_len
    ldr     w2, [x4]
    mov     x8, #__NR_write
    svc     0

    cmp     w0, 0
    b.lt    .Lwrite_failure
.endm

.macro  writecc, string, cc
    mov     w0, w15                                                 // File descriptor
    adr     x1, \string                                             // PC relative addresses
    adr     x4, \string\()_len
    ldr     w2, [x4]                                                // Length of string
    add     x7, x4, #0x04                                           // Address of n-string
    csel    x1, x1, x7, \cc                                         // Write n-string if condition not met
    mov     x8, #__NR_write
    svc     0

    cmp     w0, 0
    b.lt    .Lwrite_failure
.endm

.macro  close
    mov     w0, w15                                                 // File descriptor
    mov     x8, #__NR_close
    svc     0
.endm

.globl  main
main:
    cmp     w0, #0x01                                               // Check for output file
    b.le    .Lno_filearg

    mov     x4, #0x01                                               // Index for filename in argv
    mov     x0, #AT_FDCWD                                           // Open relative current dir
    ldr     x1, [x1, x4, lsl #0x03]                                 // Address of file name
    mov     x2, #O_WRONLY | O_CREAT | O_TRUNC                       // Flags
    mov     x3, #S_RDWR                                             // Permissions
    mov     x8, #__NR_openat                                        // Syscall openat
    svc     0
    cmp     w0, #0x00                                               // Check file descriptor
    b.lt    .Lopen_failure
    mov     w15, w0                                                 // Preserve file descriptor across syscalls

    mrs     x9, ID_AA64PFR0_EL1                                     // Read processor feature register 0
    lsr     x9, x9, #0x14                                           // Shift out low 20 bits
    and     x9, x9, #0x0f                                           // Mask out higher bits
    cmp     x9, #0x0f                                               // Check if simd is implemented
    writecc advsimd_impl_y, ne
    b.eq    .Lno_simd

    tst     x9, #0x01                                               // Check half precision floating-point support
    writecc advsimd_hp_support_y, ne

    write   advsimd_support_y

    close
    exit    0

.Lno_filearg:
    exiterr no_file

.Lopen_failure:
    exiterr open_fail

.Lwrite_failure:
    close
    exiterr write_fail

.Lno_simd:
    write   advsimd_support_n
    close
    exit    0

    .section .data
    strvtup no_file, "No filename supplied\n"
    strvtup open_fail, "Open syscall failed\n"
    strvtup write_fail, "Write syscall failed\n"

    strtup  advsimd_support, y                                      // Required simd operations supported
    strtup  advsimd_support, n                                      // Required simd operations not supported

    strtup  advsimd_hp_support, y                                   // Bits 20-23 of ID_AA64PFT0_EL1 have value 1
    strtup  advsimd_hp_support, n

    strtup  advsimd_impl, y                                         // Implemented if bits 21, 22 and 23 in ID_AA64PFT0_EL1 are not set
    strtup  advsimd_impl, n
