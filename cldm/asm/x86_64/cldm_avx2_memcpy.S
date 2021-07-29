    .section .text

.globl cldm_avx2_memcpy
# AVX2-accelerated memcpy
# Params:
#   rdi: Destination address
#   rsi: Source address
#   rdx: Number of bytes to copy
# Return:
#   rax: Destination address
cldm_avx2_memcpy:
    movq    %rdi, %rax                              # Return value

    cmpq    $0x20, %rdx                             # Check size against 32
    jb      .Lcpy16b
    je      .Lcpy32b_unaligned

    vmovdqu (%rsi), %ymm0                           # Copy first ymmword unaligned
    vmovdqu %ymm0, (%rax)

    leaq    0x0120(%rax), %rcx                      # End of offset for first 256-byte block
    andq    $-0x20, %rcx                            # Align
    subq    %rax, %rcx

    .align 16
.Lcpy256b:
    cmp     %rcx, %rdx                              # Check room for 256 bytes
    jb      .Lcpy128b

    vmovdqu -0x0100(%rsi, %rcx), %ymm0              # Copy 8 ymmwords
    vmovdqa %ymm0, -0x0100(%rax, %rcx)

    vmovdqu -0x00e0(%rsi, %rcx), %ymm1
    vmovdqa %ymm1, -0x00e0(%rax, %rcx)

    vmovdqu -0x00c0(%rsi, %rcx), %ymm2
    vmovdqa %ymm2, -0x00c0(%rax, %rcx)

    vmovdqu -0x00a0(%rsi, %rcx), %ymm3
    vmovdqa %ymm3, -0x00a0(%rax, %rcx)

    vmovdqu -0x0080(%rsi, %rcx), %ymm4
    vmovdqa %ymm4, -0x0080(%rax, %rcx)

    vmovdqu -0x0060(%rsi, %rcx), %ymm5
    vmovdqa %ymm5, -0x0060(%rax, %rcx)

    vmovdqu -0x0040(%rsi, %rcx), %ymm6
    vmovdqa %ymm6, -0x0040(%rax, %rcx)

    vmovdqu -0x0020(%rsi, %rcx), %ymm7
    vmovdqa %ymm7, -0x0020(%rax, %rcx)

    leaq    0x0100(%rcx), %rcx                      # Advance offset
    ja      .Lcpy256b                               # Repeat if more bytes remain
    vzeroupper
    ret

.Lcpy128b:
    subq    $0x80, %rcx
    cmp     %rcx, %rdx                              # Check room for 128 bytes
    jb      .Lcpy64b

    vmovdqu -0x80(%rsi, %rcx), %ymm0                # Copy 4 ymmwords
    vmovdqa %ymm0, -0x80(%rax, %rcx)

    vmovdqu -0x60(%rsi, %rcx), %ymm1
    vmovdqa %ymm1, -0x60(%rax, %rcx)

    vmovdqu -0x40(%rsi, %rcx), %ymm2
    vmovdqa %ymm2, -0x40(%rax, %rcx)

    vmovdqu -0x20(%rsi, %rcx), %ymm3
    vmovdqa %ymm3, -0x20(%rax, %rcx)

    jna     .Lepi
    leaq    0x80(%rcx), %rcx

.Lcpy64b:
    subq    $0x40, %rcx
    cmpq    %rcx, %rdx                              # Check room for 64 bytes
    jb      .Lcpy32b

    vmovdqu -0x40(%rsi, %rcx), %ymm0                # Copy 2 ymmwords
    vmovdqa %ymm0, -0x40(%rax, %rcx)

    vmovdqu -0x20(%rsi, %rcx), %ymm1
    vmovdqa %ymm1, -0x20(%rax, %rcx)

    jna     .Lepi
    leaq    0x40(%rcx), %rcx

.Lcpy32b:
    subq    $0x20, %rcx
    cmpq    %rcx, %rdx                              # Check room for 32 bytes
    jb      .Lcpylast32b

    vmovdqu -0x20(%rsi, %rcx), %ymm0                # Copy ymmword
    vmovdqa %ymm0, -0x20(%rax, %rcx)

    jna     .Lepi
    leaq    0x20(%rcx), %rcx

.Lcpylast32b:
    leaq    -0x20(%rcx), %r8                        # Set up unaligned write
    subq    %rdx, %rcx
    subq    %rcx, %r8

    vmovdqu (%rsi, %r8), %ymm0                      # Final 32 bytes
    vmovdqu %ymm0, (%rax, %r8)

.Lepi:
    vzeroupper
    ret

.Lcpy32b_unaligned:
    vmovdqu (%rsi), %ymm0                           # Copy single ymmword
    vmovdqu %ymm0, (%rax)
    vzeroupper
    ret

.Lcpy16b:
    xorl    %ecx, %ecx                              # Offset

    cmpl    $0x10, %edx                             # Check room for 16 bytes
    jb      .Lcpy8b

    vmovdqu (%rsi), %xmm0                           # Copy xmmword
    vmovdqu %xmm0, (%rax)

    je      .Lepi                                   # Done if no more bytes

    leal    -0x10(%edx), %ecx                       # Offset for final load
    vmovdqu (%rsi, %rcx), %xmm0
    vmovdqu %xmm0, (%rdi, %rcx)
    vzeroupper
    ret

.Lcpy8b:
    cmpl    $0x08, %edx                             # Check room for 8 bytes
    jb      .Lcpy4b

    vmovq   (%rsi), %xmm0                           # Copy qword
    vmovq   %xmm0, (%rdi)

    je      .Lepi                                   # Done if no more bytes

    leal    -0x08(%edx), %ecx                       # Offset for final load
    vmovq   (%rsi, %rcx), %xmm0
    vmovq   %xmm0, (%rdi, %rcx)
    vzeroupper
    ret

.Lcpy4b:
    cmpl    $0x04, %edx                             # Check room for 4 bytes
    jb      .Lcpy2b

    vmovd   (%rsi), %xmm0                           # Copy dword
    vmovd   %xmm0, (%rdi)

    je      .Lepi                                   # Done if no more bytes

    leal    -0x04(%edx), %ecx                       # Offset for final load
    vmovd   (%rsi, %rcx), %xmm0
    vmovd   %xmm0, (%rdi, %rcx)
    vzeroupper
    ret

.Lcpy2b:
    cmpl    $0x02, %edx                             # Check room for 2 bytes
    jb      .Lcpy1b

    movzxb  (%rsi), %r8d                            # Copy bytes
    movb    %r8b, (%rdi)

    movzxb  0x01(%rsi), %r9d
    movb    %r9b, 0x01(%rdi)

    je      .Lepi                                   # Done if no more bytes

    leal    0x02(%ecx), %ecx

.Lcpy1b:
    movzxb  (%rsi, %rcx), %r8d
    movb    %r8b, (%rdi, %rcx)
    ret

