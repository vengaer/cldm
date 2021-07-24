    .section .text

.globl cldm_avx2_memswp
# Swap rdx bytes between (%rdi) and (%rsi)
# Params:
#   rdi: Address of first memory area
#   rsi: Address of second memory area
#   rdx: Number of bytes to swap
# Return:
#   -
cldm_avx2_memswp:
    leaq    0x20(%rdi), %rax                        # Offset of first aligned load
    andq    $-0x20, %rax
    subq    %rdi, %rax

    leaq    0x20(%rax), %rcx                        # Check if enough space for aligning
    cmpq    %rcx, %rdx
    jna     .Lswp32b_unaligned

    vmovdqu (%rdi), %ymm0                           # Load first ymmwords unaligned
    vmovdqu (%rsi), %ymm1

    vmovdqa (%rdi, %rax), %ymm2                     # Load overlapping bytes aligned
    vmovdqu (%rsi, %rax), %ymm3

    vmovdqu %ymm1, (%rdi)                           # Write back first set of ymmwords
    vmovdqu %ymm0, (%rsi)

    vmovdqa %ymm3, (%rdi, %rax)                     # Write back second set of ymmword
    vmovdqu %ymm2, (%rsi, %rax)

    leaq    0xa0(%rax), %rax                        # End of offset for first 128 byte block

    .align 16
.Lswp128b:
    cmpq    %rax, %rdx                              # Check against upper bound
    jb      .Lswp64b

    vmovdqa -0x80(%rdi, %rax), %ymm0                # Swap 4 ymmwords
    vmovdqu -0x80(%rsi, %rax), %ymm1
    vmovdqa %ymm1, -0x80(%rdi, %rax)
    vmovdqu %ymm0, -0x80(%rsi, %rax)

    vmovdqa -0x60(%rdi, %rax), %ymm2
    vmovdqu -0x60(%rsi, %rax), %ymm3
    vmovdqa %ymm3, -0x60(%rdi, %rax)
    vmovdqu %ymm2, -0x60(%rsi, %rax)

    vmovdqa -0x40(%rdi, %rax), %ymm3
    vmovdqu -0x40(%rsi, %rax), %ymm4
    vmovdqa %ymm4, -0x40(%rdi, %rax)
    vmovdqu %ymm3, -0x40(%rsi, %rax)

    vmovdqa -0x20(%rdi, %rax), %ymm5
    vmovdqu -0x20(%rsi, %rax), %ymm6
    vmovdqa %ymm6, -0x20(%rdi, %rax)
    vmovdqu %ymm5, -0x20(%rsi, %rax)

    leaq    0x80(%rax), %rax                        # Advance
    ja      .Lswp128b
    vzeroupper
    ret

.Lswp64b:
    subq    $0x40, %rax
    cmpq    %rax, %rdx                              # Check against upper bound
    jb      .Lswp32b

    vmovdqa -0x40(%rdi, %rax), %ymm0                # Swap 2 ymmwords
    vmovdqu -0x40(%rsi, %rax), %ymm1
    vmovdqa %ymm1, -0x40(%rdi, %rax)
    vmovdqu %ymm0, -0x40(%rsi, %rax)

    vmovdqa -0x20(%rdi, %rax), %ymm2
    vmovdqu -0x20(%rsi, %rax), %ymm3
    vmovdqa %ymm3, -0x20(%rdi, %rax)
    vmovdqu %ymm2, -0x20(%rsi, %rax)

    je      .Lepi
    leaq    0x40(%rax), %rax

.Lswp32b:
    subq    $0x20, %rax
    cmpq    %rax, %rdx                              # Check against upper bound
    jb      .Lswp16b

    vmovdqa -0x20(%rdi, %rax), %ymm0                # Load first set of ymmwords
    vmovdqu -0x20(%rsi, %rax), %ymm1

    vmovdqu -0x20(%rdi, %rdx), %ymm2                # Final set of ymmwords
    vmovdqu -0x20(%rsi, %rdx), %ymm3

    vmovdqa %ymm1, -0x20(%rdi, %rax)                # First set of ymmwords
    vmovdqu %ymm0, -0x20(%rsi, %rax)

    je      .Lepi                                   # Done if no more bytes

    vmovdqu %ymm3, -0x20(%rdi, %rdx)                # Write final set of ymmwords
    vmovdqu %ymm2, -0x20(%rsi, %rdx)
    vzeroupper
    ret

.Lswp16b:
    subq    $0x10, %rax
    cmpq    %rax, %rdx                              # Check against upper bound
    jb      .Lswp8b

    vmovdqa -0x10(%rdi, %rax), %xmm0                # First set of xmmwords
    vmovdqu -0x10(%rsi, %rax), %xmm1

    vmovdqu -0x10(%rdi, %rdx), %xmm2                # Final set of xmmwords
    vmovdqu -0x10(%rsi, %rdx), %xmm3

    vmovdqa %xmm1, -0x10(%rdi, %rax)                # Write first set of xmmwords
    vmovdqu %xmm0, -0x10(%rsi, %rax)

    je      .Lepi                                   # Done if no more bytes

    vmovdqu %xmm3, -0x10(%rdi, %rdx)                # Write second set of xmmwords
    vmovdqu %xmm2, -0x10(%rsi, %rdx)

    vzeroupper
    ret

.Lswp8b:
    subq    $0x08, %rax
    cmpq    %rax, %rdx                              # Check against upper bound
    jb      .Lswp4b

    vmovq   -0x08(%rdi, %rax), %xmm0                # First set of qwords
    vmovq   -0x08(%rsi, %rax), %xmm1

    vmovq   -0x08(%rdi, %rdx), %xmm2                # Final set of qwords
    vmovq   -0x08(%rsi, %rdx), %xmm3

    vmovq   %xmm1, -0x08(%rdi, %rax)                # Write back first set of qwords
    vmovq   %xmm0, -0x08(%rsi, %rax)

    je      .Lepi                                   # Done if no more bytes

    vmovq   %xmm3, -0x08(%rdi, %rdx)                # Write final set of qwords
    vmovq   %xmm2, -0x08(%rsi, %rdx)

    vzeroupper
    ret

.Lswp4b:
    subq    $0x04, %rax
    cmpq    %rax, %rdx                              # Check against upper bound
    jb      .Lswp2b

    vmovd   -0x04(%rdi, %rax), %xmm0                # First set of dwords
    vmovd   -0x04(%rsi, %rax), %xmm1

    vmovd   -0x04(%rdi, %rdx), %xmm2                # Second set of dwords
    vmovd   -0x04(%rsi, %rdx), %xmm3

    vmovd   %xmm1, -0x04(%rdi, %rax)                # Write back first set of dwords
    vmovd   %xmm0, -0x04(%rsi, %rax)

    je      .Lepi                                   # Done if no more bytes

    vmovd   %xmm3, -0x04(%rdi, %rdx)                # Write final set of dwords
    vmovd   %xmm2, -0x04(%rsi, %rdx)

    vzeroupper
    ret

.Lswp2b:
    subq    $0x02, %rax
    cmpq    %rax, %rdx                              # Check against upper bound
    jb      .Lswp1b

    movzxb  -0x02(%rdi, %rax), %ecx
    movzxb  -0x02(%rsi, %rax), %r8d
    movb    %r8b, -0x02(%rdi, %rax)
    movb    %cl, -0x02(%rsi, %rax)

    movzxb  -0x01(%rdi, %rax), %ecx
    movzxb  -0x01(%rsi, %rax), %r8d
    movb    %r8b, -0x01(%rdi, %rax)
    movb    %cl, -0x01(%rsi, %rax)

    je      .Lepi                                   # Done if no more bytes

    leaq    0x02(%rax), %rax

.Lswp1b:
    subq    $0x01, %rax
    movzxb  -0x01(%rdi, %rax), %ecx
    movzxb  -0x01(%rsi, %rax), %r8d
    movb    %r8b, -0x01(%rdi, %rax)
    movb    %cl, -0x01(%rsi, %rax)

.Lepi:
    vzeroupper
    ret

.Lswp32b_unaligned:
    cmpl    $0x20, %edx
    jb      .Lswp16b_unaligned

    vmovdqu (%rdi), %ymm0                           # Swap single ymmword
    vmovdqu (%rsi), %ymm1

    vmovdqu -0x20(%rdi, %rdx), %ymm2                # Load final ymmword
    vmovdqu -0x20(%rsi, %rdx), %ymm3

    vmovdqu %ymm1, (%rdi)                           # Write first pair of ymmwords
    vmovdqu %ymm0, (%rsi)

    je      .Lepi

    vmovdqu %ymm3, -0x20(%rdi, %rdx)                # Write final pair of ymmwords
    vmovdqu %ymm2, -0x20(%rsi, %rdx)

    vzeroupper
    ret

.Lswp16b_unaligned:
    cmpl    $0x10, %edx                             # Check against upper bound
    jb      .Lswp8b_unaligned

    vmovdqu (%rdi), %xmm0                           # Swap single xmmword
    vmovdqu (%rsi), %xmm1

    vmovdqu -0x10(%rdi, %rdx), %xmm2                # Load final xmmword before writing first ones back
    vmovdqu -0x10(%rsi, %rdx), %xmm3

    vmovdqu %xmm1, (%rdi)                           # Write first pair of xmmwords
    vmovdqu %xmm0, (%rsi)

    je      .Lepi                                   # Done if no more bytes

    vmovdqu %xmm3, -0x10(%rdi, %rdx)                # Write final pair of xmmwords
    vmovdqu %xmm2, -0x10(%rsi, %rdx)

    vzeroupper
    ret

.Lswp8b_unaligned:
    cmpl    $0x08, %edx                             # Check against upper bound
    jb      .Lswp4b_unaligned

    leal    -0x08(%edx), %eax                       # Offset for final swap

    vmovq   (%rdi), %xmm0
    vmovq   (%rsi), %xmm1

    vmovq   -0x08(%rdi, %rdx), %xmm2                # Load final set of qwords
    vmovq   -0x08(%rsi, %rdx), %xmm3

    vmovq   %xmm1, (%rdi)                           # Write back first set of qwords
    vmovq   %xmm0, (%rsi)

    je      .Lepi                                   # Done if no more bytes

    vmovq   %xmm3, -0x08(%rdi, %rdx)                # Write last set of qwords
    vmovq   %xmm2, -0x08(%rsi, %rdx)

    vzeroupper
    ret

.Lswp4b_unaligned:
    cmpl    $0x04, %edx                             # Check against upper bound
    jb      .Lswp2b_unaligned

    vmovd   (%rdi), %xmm0
    vmovd   (%rsi), %xmm1

    vmovd   -0x04(%rdi, %rdx), %xmm2                # Load final set of dwords
    vmovd   -0x04(%rsi, %rdx), %xmm3

    vmovd   %xmm1, (%rdi)                           # Write first set of dwords
    vmovd   %xmm0, (%rsi)

    je      .Lepi                                   # Done if no more bytes

    vmovd   %xmm3, -0x04(%rdi, %rdx)                # Write final set of dwords
    vmovd   %xmm2, -0x04(%rsi, %rdx)

    vzeroupper
    ret

.Lswp2b_unaligned:
    xorl    %eax, %eax                              # Offset

    cmpl    $0x02, %edx                             # Check against upper bound
    jb      .Lswp1b_unaligned

    movzxb  (%rdi), %ecx
    movzxb  (%rsi), %r8d
    movb    %r8b, (%rdi)
    movb    %cl, (%rsi)

    movzxb  0x01(%rdi), %ecx
    movzxb  0x01(%rsi), %r8d
    movb    %r8b, 0x01(%rdi)
    movb    %cl, 0x01(%rsi)

    je      .Lepi                                   # Done if no more bytes

    leal    0x02(%eax), %eax

.Lswp1b_unaligned:
    movzxb  (%rdi, %rax), %ecx
    movzxb  (%rsi, %rax), %r8d
    movb    %r8b, (%rdi, %rax)
    movb    %cl, (%rsi, %rax)
    vzeroupper
    ret
