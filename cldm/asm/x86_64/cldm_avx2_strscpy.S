    .section .text

    #include "asm/system.h"

    #define E2BIG 7

.macro vecoffs vecreg, baseoff
    vpmovmskb   \vecreg, %ecx                       # Extract bitmask
    tzcntl  %ecx, %r8d                              # Offset of null byte in ymmword
    leaq    \baseoff(%rax, %r8), %rax
.endm

.globl cldm_avx2_strscpy
# AVX2-accelerated string copy routine.
# Provided that %rdx is at least 1, the
# result is # guaranteed to be null
# terminated. If (%rsi) is longer than %rdx
# bytes, %rdx - 1 # bytes of (%rsi) are
# written to (%rdi)
#
# (%rsi) and (%rdi) may not overlap
#
# Accesses beyond the null terminator in
# (%rsi) are, by design, likely to occur.
# No accesses, neither reads nor writes,
# risk hitting the guard page.
#
# Params:
#   rdi: Address of destination
#   rsi: Address of source
#   rdx: Size of destination
# Return:
#   rax: Size of the string written to (%rdi),
#        or -7 if the length of (%rsi) is
#        greater than %rdx
cldm_avx2_strscpy:
    testq   %rdx, %rdx                              # Check for empty destination
    jz      .Lepi_empty

    xorl    %eax, %eax                              # Offset

    vpxor   %ymm15, %ymm15, %ymm15

    movl    %esi, %ecx                              # Check whether reading ymmword would cross page boundary
    andl    $PGSIZE - 1, %ecx
    cmpl    $PGSIZE - 0x20, %ecx
    ja      .Lpgx_unaligned

    cmpq    $0x20, %rdx
    jb      .Lbufend_unaligned

    vmovdqu (%rsi), %ymm0                           # Load first ymmword
    vpcmpeqb    %ymm15, %ymm0, %ymm4                # Compare to null
    vmovdqu %ymm0, (%rdi)                           # Store ymmword

    vptest  %ymm4, %ymm4                            # Check if there was a null byte
    jz      .L32balign

    vpmovmskb   %ymm4, %ecx                         # Extract bitmask
    tzcntl  %ecx, %eax                              # Length is index of first null byte
    vzeroupper
    ret

.L32balign:
    leaq    0x20(%rsi), %rax                        # Align reads to 32 byte boundary
    andq    $-0x20, %rax
    subq    %rsi, %rax

    .align 16
.Lcpy128b_prlg:
    leaq    (%rsi, %rax), %rcx                      # Check whether reading 4 ymmwords would cross boundary
    andl    $PGSIZE - 1, %ecx
    cmpl    $PGSIZE - 0x80, %ecx
    ja      .Lpgx32b

.Lcpy128b:
    leaq    0x80(%rax), %rcx                        # Check for end of buffer
    cmpq    %rdx, %rcx
    ja      .Lbufend_aligned

    vmovdqa 0x00(%rsi, %rax), %ymm0                 # Load 3 ymmwords and compare to null
    vpcmpeqb    %ymm15, %ymm0, %ymm4
    vmovdqa 0x20(%rsi, %rax), %ymm1
    vpcmpeqb    %ymm15, %ymm1, %ymm5
    vmovdqa 0x40(%rsi, %rax), %ymm2
    vpcmpeqb    %ymm15, %ymm2, %ymm6
    vmovdqa 0x60(%rsi, %rax), %ymm3
    vpcmpeqb    %ymm15, %ymm3, %ymm7

    vpor    %ymm4, %ymm5, %ymm8                     # Reduce
    vpor    %ymm6, %ymm7, %ymm9
    vpor    %ymm8, %ymm9, %ymm10

    vmovdqu %ymm0, 0x00(%rdi, %rax)                 # First ymmword always written back

    vptest  %ymm10, %ymm10                          # Clear ZF if null byte was encountered
    jnz     .Lnull128b

    vmovdqu %ymm1, 0x20(%rdi, %rax)                 # Write remaining ymmwords
    vmovdqu %ymm2, 0x40(%rdi, %rax)
    vmovdqu %ymm3, 0x60(%rdi, %rax)

    leaq    0x80(%rax), %rax
    jmp     .Lcpy128b_prlg

.Lnull128b:
    vptest  %ymm8, %ymm8                            # Check if null byte in low zmmword
    jnz     .Lnull64b

    vmovdqu %ymm1, 0x20(%rdi, %rax)                 # Null in either third or fourth ymmword, at least 3 ymmwords written
    vmovdqu %ymm2, 0x40(%rdi, %rax)

    vptest  %ymm6, %ymm6                            # Clear ZF if null in third ymmword
    jnz     .Lnull96b

    vmovdqu %ymm3, 0x60(%rdi, %rax)                 # Null byte in fourth ymmword, write back

    vecoffs %ymm7, 0x60                             # Compute return value
    vzeroupper
    ret

.Lnull64b:
    vptest  %ymm4, %ymm4                            # Clear ZF if null byte is in first ymmword
    jnz     .Lnull32b

    vmovdqu %ymm1, 0x20(%rdi, %rax)                 # Null byte in second ymmword, store it

    vecoffs %ymm5, 0x20                             # Compute return value
    vzeroupper
    ret

.Lnull96b:
    vecoffs %ymm6, 0x40                             # Compute return value
    vzeroupper
    ret

.Lnull32b:
    vecoffs %ymm4, 0x00                             # Compute return value
    vzeroupper
    ret

.Lpgx32b:
    leaq    0x20(%rax), %rcx                        # Check against end of buffer
    cmpq    %rdx, %rcx
    ja      .Lpgx32b_bufend

    vmovdqa (%rsi, %rax), %ymm0                     # Load, compare to null and store
    vpcmpeqb    %ymm15, %ymm0, %ymm4
    vmovdqu %ymm0, (%rdi, %rax)

    leaq    0x20(%rax), %rax                        # Advance offset

    vptest  %ymm4, %ymm4                            # Clear ZF if null byte was encountered
    jnz     .Lpgx32b_epi

    leaq    (%rsi, %rax), %rcx                      # Check if aligned to page boundary
    andl    $PGSIZE - 1, %ecx
    jz      .Lcpy128b
    jmp     .Lpgx32b

.Lpgx32b_bufend:
    movq    %rdx, %rcx                              # Compute number of bytes to retract offset
    subq    %rax, %rcx
    negq    %rcx                                    # Multiply by -1
    leaq    0x20(%rcx), %r10
    jmp     .Lbufend32b

.Lpgx32b_epi:
    vecoffs %ymm4, -0x20                            # Compute return value
    vzeroupper
    ret

.Lbufend_aligned:
    leaq    .Lendtbl(%rip), %r8                     # Load jump table
    movq    %rdx, %rcx                              # Compute number of remaining bytes in buffer
    subq    %rax, %rcx

    movl    %ecx, %r9d                              # Compute offset
    shrl    $0x05, %r9d                             # Divide by 32

    andl    $0x1f, %ecx                             # Compute number of bytes to retract
    negl    %ecx
    leal    0x20(%ecx), %r10d

    jmp     *(%r8, %r9, 0x08)                       # Jump to branch

.Lbufend32b:
    subq    %r10, %rax                              # Set up offset for final load

    vmovdqu (%rsi, %rax), %ymm0                     # Load, compare and store last ymmword
    vpcmpeqb    %ymm15, %ymm0, %ymm4
    vmovdqu %ymm0, (%rdi, %rax)

    vptest  %ymm4, %ymm4                            # Clear ZF if null byte was encountered
    jz      .Lepi_ovfz

    vecoffs %ymm4, 0x00                             # Compute return value
    vzeroupper
    ret

.Lbufend64b:
    vmovdqa (%rsi, %rax), %ymm0                     # Load, comapre and store first ymmword
    vpcmpeqb    %ymm15, %ymm0, %ymm4
    vmovdqu %ymm0, (%rdi, %rax)

    vptest  %ymm4, %ymm4                            # Clear ZF if null byte was encountered
    jz      .Lbufend64b_final

    vecoffs %ymm4, 0x00                             # Add offset of null byte in ymmword to %rax
    vzeroupper
    ret

.Lbufend64b_final:
    leaq    0x20(%rax), %rax                        # Set up offset for final load
    subq    %r10, %rax

    vmovdqu (%rsi, %rax), %ymm1                     # Load, compare and store final ymmword
    vpcmpeqb    %ymm15, %ymm1, %ymm5
    vmovdqu %ymm1, (%rdi, %rax)

    vptest  %ymm5, %ymm5                            # Clear ZF if null byte was encountered
    jz      .Lepi_ovfz

    vecoffs %ymm5, 0x00                             # Compute return value
    vzeroupper
    ret

.Lbufend96b:
    vmovdqa (%rsi, %rax), %ymm0                     # Load, compare and store first ymmword
    vpcmpeqb    %ymm15, %ymm0, %ymm4
    vmovdqu %ymm0, (%rdi, %rax)

    vptest  %ymm4, %ymm4                            # Clear ZF if null byte was encountered
    jz      .Lbufend96b_mid

    vecoffs %ymm4, 0x00                             # Compute return value
    vzeroupper
    ret

.Lbufend96b_mid:
    vmovdqa 0x20(%rsi, %rax), %ymm1                 # Load, compare and store second ymmword
    vpcmpeqb    %ymm15, %ymm1, %ymm5
    vmovdqu %ymm1, 0x20(%rdi, %rax)

    vptest  %ymm5, %ymm5                            # Clear ZF if null byte was encountered
    jz      .Lbufend96b_final

    vecoffs %ymm5, 0x20                             # Compute return value
    vzeroupper
    ret

.Lbufend96b_final:
    leaq    0x40(%rax), %rax                        # Set up offset for final load
    subq    %r10, %rax

    vmovdqu (%rsi, %rax), %ymm2                     # Load, compare and store final ymmword
    vpcmpeqb    %ymm15, %ymm2, %ymm6
    vmovdqu %ymm2, (%rdi, %rax)

    vptest  %ymm6, %ymm6                            # Clear ZF if null byte was encountered
    jz      .Lepi_ovfz

    vecoffs %ymm6, 0x00                             # Compute return value
    vzeroupper
    ret

.Lbufend128b:
    vmovdqa (%rsi, %rax), %ymm0                     # Load, compare and store first ymmword
    vpcmpeqb    %ymm15, %ymm0, %ymm4
    vmovdqu %ymm0, (%rdi, %rax)

    vptest  %ymm4, %ymm4                            # Clear ZF if null byte was encountered
    jz      .Lbufend128b_second

    vecoffs %ymm4, 0x00
    vzeroupper
    ret

.Lbufend128b_second:
    vmovdqa 0x20(%rsi, %rax), %ymm1                 # Load, compare and store second ymmword
    vpcmpeqb    %ymm15, %ymm1, %ymm5
    vmovdqu %ymm1, 0x20(%rdi, %rax)

    vptest  %ymm5, %ymm5                            # Clear ZF if null byte was encountered
    jz      .Lbufend128b_third

    vecoffs %ymm5, 0x20
    vzeroupper
    ret

.Lbufend128b_third:
    vmovdqa  0x40(%rsi, %rax), %ymm2                # Load, compare and store third ymmword
    vpcmpeqb    %ymm15, %ymm2, %ymm6
    vmovdqu %ymm2, 0x40(%rdi, %rax)

    vptest  %ymm6, %ymm6                            # Clear ZF if null byte was encountered
    jz      .Lbufend128b_final

    vecoffs %ymm6, 0x40
    vzeroupper
    ret

.Lbufend128b_final:
    leaq    0x60(%rax), %rax                        # Set up offset for final load
    subq    %r10, %rax

    vmovdqu (%rsi, %rax), %ymm3                     # Load, compare and store final ymmword
    vpcmpeqb    %ymm15, %ymm3, %ymm7
    vmovdqu %ymm3, (%rdi, %rax)

    vptest  %ymm7, %ymm7                            # Clear ZF if null byte was encountered
    jz      .Lepi_ovfz

    vecoffs %ymm7, 0x00
    vzeroupper
    ret

.Lpgx_unaligned:
    negl    %ecx
    leal    PGSIZE(%ecx), %r9d
    cmpq    %rdx, %r9                               # Check whether buffer ends before memory page does
    jnb     .Lbufend_unaligned

    leaq    .Lpgxtbl(%rip), %r8                     # Load jump table
    lzcntl  %r9d, %ecx                              # Number of leading zero bits
    negq    %rcx                                    # Multiply by -1
    jmp     *0x08 * 0x1f(%r8, %rcx, 0x08)           # Jump to address %r8 + (%rcx + $0x1f) * $0x08, %rcx + $0x1f -> most significant set bit in %r9d, zero-indexed

.Lpgx1b:
    movzxb  (%rsi), %ecx                            # Copy byte
    movb    %cl, (%rdi)

    testl   %ecx, %ecx                              # Check for null
    setnzb  %al                                     # Conditionally advance offset
    jnz     .Lpgx_bufchk
    ret

.Lpgx2b:
    movzxb  0x00(%rsi), %eax                        # Copy first 2 bytes
    movb    %al, 0x00(%rdi)
    movzxb  0x01(%rsi), %ecx
    movb    %cl, 0x01(%rdi)

    xorl    %r11d, %r11d
    movl    $0xff, %r10d
    testl   %eax, %eax
    cmovnzl %r10d, %r11d                            # Set all bits in low byte if first byte is not null

    testl   %r11d, %ecx                             # Set ZF if either byte was null
    jnz     .Lpgx2b_final
    movl    $0x01, %r10d
    testl   %eax, %eax
    cmovnz  %r10d, %eax                             # Return 1 if first byte is non-zero
    ret

.Lpgx2b_final:
    movl    $0x02, %eax                             # Offset
    cmpl    %eax, %r9d                              # Check if aligned to page boundary
    je      .Lpgx_bufchk

    movzxb  0x02(%rsi), %ecx                        # Copy third byte
    movb    %cl, 0x02(%rdi)

    movl    $0x03, %r9d                             # Incremented size
    testl   %ecx, %ecx
    cmovnzl %r9d, %eax                              # If not null, increment size and jump
    jnz     .Lpgx_bufchk
    ret

.Lpgx4b:
    vmovd   (%rsi), %xmm0                           # Load, compare and store first dword
    vpcmpeqb    %xmm15, %xmm0, %xmm4
    vmovd   %xmm0, (%rdi)

    vpmovmskb   %xmm4, %ecx                         # Check for null in low 4 bytes
    tzcntl  %ecx, %eax
    cmpl    $0x04, %eax
    jnb     .Lpgx4b_final
    vzeroupper
    ret

.Lpgx4b_final:
    movl    $0x04, %eax                             # Offset
    cmpl    %eax, %r9d                              # Check if aligned to boundary
    je      .Lpgx_bufchk

    movl    %r9d, %ecx                              # Set up offset for final dword
    subl    %eax, %ecx
    leal    -0x04(%eax, %ecx), %eax

    vmovd   (%rsi, %rax), %xmm1                     # Load, compare and store final dword
    vpcmpeqb    %xmm15, %xmm1, %xmm5
    vmovd   %xmm1, (%rdi, %rax)

    vpmovmskb   %xmm5, %ecx                         # Check for null in low 4 bytes
    tzcntl  %ecx, %r8d
    leal    (%eax, %r8d), %eax
    cmpl    $0x04, %r8d
    jnb     .Lpgx_bufchk
    vzeroupper
    ret

.Lpgx8b:
    vmovq   (%rsi), %xmm0                           # Load, compare and store first qword
    vpcmpeqb    %xmm15, %xmm0, %xmm4
    vmovq   %xmm0, (%rdi)

    vpmovmskb   %xmm4, %ecx                         # Check for null in low 8 bytes
    tzcntl  %ecx, %eax
    cmpl    $0x08, %eax
    jnb     .Lpgx8b_final
    vzeroupper
    ret

.Lpgx8b_final:
    movl    $0x08, %eax                             # Offset
    cmpl    %eax, %r9d                              # Check if aligned to page
    je      .Lpgx_bufchk

    movl    %r9d, %ecx                              # Set up offset for final qword
    subl    %eax, %ecx
    leal    -0x08(%eax, %ecx), %eax

    vmovq   (%rsi, %rax), %xmm1                     # Load, compare and store final qword
    vpcmpeqb    %xmm15, %xmm1, %xmm5
    vmovq   %xmm1, (%rdi, %rax)

    vpmovmskb   %xmm5, %ecx
    tzcntl  %ecx, %r8d
    leal    (%eax, %r8d), %eax
    cmpl    $0x08, %r8d
    jnb     .Lpgx_bufchk
    vzeroupper
    ret

.Lpgx16b:
    vmovdqu (%rsi), %xmm0                           # Load, compare and store first xmmword
    vpcmpeqb    %xmm15, %xmm0, %xmm4
    vmovdqu %xmm0, (%rdi)

    vptest  %xmm4, %xmm4                            # Clear ZF if null byte was encountered
    jz      .Lpgx16b_final
    vpmovmskb   %xmm4, %ecx
    tzcntl  %ecx, %eax
    vzeroupper
    ret

.Lpgx16b_final:
    movl    $0x10, %eax                             # Offset
    cmpl    %eax, %r9d                              # Check if aligned to page
    je      .Lpgx_bufchk

    movl    %r9d, %ecx                              # Set up offset for final xmmword
    subl    %eax, %ecx
    leal    -0x10(%eax, %ecx), %eax

    vmovdqu (%rsi, %rax), %xmm1                     # Load, compare and store final xmmword
    vpcmpeqb    %xmm15, %xmm1, %xmm5
    vmovdqu %xmm1, (%rdi, %rax)

    leal    0x10(%eax), %eax                        # Advance offset

    vptest  %xmm5, %xmm5                            # Clear ZF if null byte was encountered
    jz      .Lpgx_bufchk

    vecoffs %xmm5, -0x10                            # Compute return value
    vzeroupper
    ret

.Lpgx_bufchk:
    leal    0x20(%eax), %ecx                        # Check if writing single ymmword would pass end of buffer
    cmpq    %rcx, %rdx
    jb      .Lbufend_unaligned

    vmovdqa (%rsi, %rax), %ymm0                     # Load, compare and store single ymmword
    vpcmpeqb    %ymm15, %ymm0, %ymm4
    vmovdqu %ymm0, (%rdi, %rax)

    vptest  %ymm4, %ymm4                            # Clear ZF if null byte was encountered
    jz      .Lcpy128b                               # Offset in page is at most 32, no need to check

    vecoffs %ymm4, 0x00                             # Compute return value
    vzeroupper
    ret

.Lbufend_unaligned:
    leaq    .Lbufendtbl(%rip), %r8                  # Load jump table
    movl    %edx, %r9d
    subl    %eax, %r9d
    lzcntl  %r9d, %ecx                              # Number of leading zero bits
    negq    %rcx                                    # Multiply by -1
    jmp     *0x08 * 0x1f(%r8, %rcx, 0x08)           # Jump to address %r8 + (%rcx + $0x1f) * $0x08, %rcx + $0x1f -> most significant set bit in %edx, zero-indexed

.Lbufend1b:
    movzxb  (%rsi, %rax), %r8d                      # Load byte
    movb    $0x00, (%rdi, %rax)                     # Null terminate
    movq    $-E2BIG, %rcx
    testl   %r8d, %r8d
    cmovnzq %rcx, %rax                              # Return -E2BIG if not null
    vzeroupper
    ret

.Lbufend2b:
    movzxb  0x00(%rsi, %rax), %r8d                  # First byte
    movb    %r8b, 0x00(%rdi, %rax)
    movzxb  0x01(%rsi, %rax), %ecx                  # Second byte
    movb    %cl, 0x01(%rdi, %rax)

    xorl    %r11d, %r11d
    movl    $0xff, %r10d
    testl   %r8d, %r8d
    cmovnz  %r10d, %r11d                            # Set low byte to all ones if first byte is not null

    testl   %r11d, %ecx                             # Set ZF if either byte was null
    jnz     .Lbufend2b_final

    andl    $0x01, %r11d                            # Increment if first byte is non-zero
    leal    (%eax, %r11d), %eax
    vzeroupper
    ret

.Lbufend2b_final:
    leal    0x02(%eax), %eax
    cmpl    %eax, %edx                              # Check for end of buffer
    jna     .Lepi_ovfz

    movzxb  (%rsi, %rax), %edx                      # Final byte
    movb    $0x00, (%rdi, %rax)                     # Null terminate
    movq    $-E2BIG, %rcx                           # Return value if final byte is non-null
    testl   %edx, %edx                              # Check if null
    cmovnzq %rcx, %rax                              # Return -E2BIG if not null
    vzeroupper
    ret

.Lbufend4b:
    vmovd   (%rsi, %rax), %xmm0                     # Load, compare and store first dword
    vpcmpeqb    %xmm15, %xmm0, %xmm4
    vmovd   %xmm0, (%rdi, %rax)

    vpmovmskb   %xmm4, %ecx                         # Check for null in low dword
    tzcntl  %ecx, %r8d
    cmpl    $0x04, %r8d
    jnb     .Lbufend4b_final

    leal    (%eax, %r8d), %eax
    vzeroupper
    ret

.Lbufend4b_final:
    leal    0x04(%eax), %r9d
    cmpl    %r9d, %edx                              # Check for end of buffer
    jna     .Lepi_ovfz

    movl    $0x04, %ecx                             # Set up offset for final dword
    movl    %edx, %eax
    subl    %ecx, %eax

    vmovd   (%rsi, %rax), %xmm1                     # Load, compare and store final dword
    vpcmpeqb    %xmm15, %xmm1, %xmm5
    vmovd   %xmm1, (%rdi, %rax)

    vpmovmskb   %xmm5, %ecx                         # Check for null byte in low 4 lanes
    tzcntl  %ecx, %r8d
    cmpl    $0x04, %r8d                             # Check if null byte in low dword
    jnb     .Lepi_ovfz
    leal    (%eax, %r8d), %eax
    vzeroupper
    ret

.Lbufend8b:
    vmovq   (%rsi, %rax), %xmm0                     # Load, c ompare and store first qword
    vpcmpeqb    %xmm15, %xmm0, %xmm4
    vmovq   %xmm0, (%rdi, %rax)

    vpmovmskb   %xmm4, %ecx                         # Check for null byte in low 8 lanes
    tzcntl  %ecx, %r8d
    cmpl    $0x08, %r8d                             # Check if null byte in low qword
    jnb     .Lbufend8b_final

    leal    (%eax, %r8d), %eax
    vzeroupper
    ret

.Lbufend8b_final:
    leal    0x08(%eax), %r9d
    cmpl    %r9d, %edx                              # Check for end of buffer
    jna     .Lepi_ovfz

    movl    $0x08, %ecx                             # Set up offset for final qword
    movl    %edx, %eax
    subl    %ecx, %eax

    vmovq   (%rsi, %rax), %xmm1                     # Load, compare and store final qword
    vpcmpeqb    %xmm15, %xmm1, %xmm5
    vmovq   %xmm1, (%rdi, %rax)

    vpmovmskb   %xmm5, %r8d                         # Check for null byte in low qword
    tzcntl  %r8d, %ecx
    cmpl    $0x08, %ecx
    jnb     .Lepi_ovfz
    leal    (%eax, %ecx), %eax
    vzeroupper
    ret

.Lbufend16b:
    vmovdqu (%rsi, %rax), %xmm0                     # Load, compare and store first xmmword
    vpcmpeqb    %xmm15, %xmm0, %xmm4
    vmovdqu %xmm0, (%rdi, %rax)

    vptest  %xmm4, %xmm4                            # Clear ZF if null byte was encountered
    jz      .Lbufend16b_final

    vpmovmskb   %xmm4, %ecx                         # Compute return value
    tzcntl  %ecx, %edx
    leal    (%eax, %edx), %eax
    vzeroupper
    ret

.Lbufend16b_final:
    leal    0x10(%eax), %r9d
    cmpl    %r9d, %edx                              # Check for end of buffer
    jna     .Lepi_ovfz

    movl    $0x10, %ecx                             # Set up offset for final xmmword
    movl    %edx, %eax
    subl    %ecx, %eax

    vmovdqu (%rsi, %rax), %xmm1                     # Load, compare and store final xmmword
    vpcmpeqb    %xmm15, %xmm1, %xmm5
    vmovdqu %xmm1, (%rdi, %rax)

    vptest  %xmm5, %xmm5                            # Clear ZF if null byte was encountered
    jz      .Lepi_ovfz

    vpmovmskb   %xmm5, %ecx                         # Compute return value
    tzcntl  %ecx, %edx
    leal    (%eax, %edx), %eax
    vzeroupper
    ret

.Lepi_ovfz:
    mov     $-E2BIG, %rax                           # Return -E2BIG
    movb    $0x00, -0x01(%rdi, %rdx)                # Null terminate
    vzeroupper
    ret

.Lepi_empty:
    movq    $-E2BIG, %rax
    ret

    .section .data
    .align 8
.Lendtbl:
    .quad   .Lbufend32b
    .quad   .Lbufend64b
    .quad   .Lbufend96b
    .quad   .Lbufend128b

.Lpgxtbl:
    .quad   .Lpgx1b
    .quad   .Lpgx2b
    .quad   .Lpgx4b
    .quad   .Lpgx8b
    .quad   .Lpgx16b

.Lbufendtbl:
    .quad   .Lbufend1b
    .quad   .Lbufend2b
    .quad   .Lbufend4b
    .quad   .Lbufend8b
    .quad   .Lbufend16b
