    .section .text

.macro  resjmp
    cmpl    %r10d, %edi                             # Check if done
    jnb     .Lresepi

    tzcntl  %edi, %ecx                              # Index of least significant set bit
    cmpl    0x2, %ecx                               # Check if 8- byte aligned
    cmoval  %r8d, %ecx                              # Clamp jump offset

    jmp     *(%r9, %rcx, 8)
.endm

.globl cldm_avx2_memset
# AVX2-accelerated memset
# Params:
#   rdi: Destination address
#   esi: Value to write to each destination
#        byte
#   rdx: Number of bytes to write
# Return:
#   rax: Destination address
cldm_avx2_memset:
    movq    %rdi, %rax                              # Return address
    cmpq    $0x20, %rdx                             # Check space for 32 bytes
    jb      .Lwr16b
    je      .Lwr32b_unaligned

    vpxor   %xmm2, %xmm2, %xmm2                     # Shuffle mask
    vmovd   %esi, %xmm1                             # Insert dword
    vpshufb %xmm2, %xmm1, %xmm0                     # Broadcast 0th byte
    vinserti128 $0x1, %xmm0, %ymm0, %ymm1           # Copy low xmmword to high

    vmovdqu %ymm1, (%rdi)                           # Write ymmword

    leaq    0x011f(%rdi), %rdi                      # Align to 32-byte boundary 256 bytes ahead
    andq    $-0x20, %rdi

    leaq    (%rax, %rdx), %rdx                      # Address of last byte to set

    .align 16
.Lwr256b:
    cmpq    %rdi, %rdx
    jb      .Lwr128b

    vmovdqa %ymm1, -0x0100(%rdi)                    # Write 256 bytes
    vmovdqa %ymm1, -0x00e0(%rdi)
    vmovdqa %ymm1, -0x00c0(%rdi)
    vmovdqa %ymm1, -0x00a0(%rdi)
    vmovdqa %ymm1, -0x0080(%rdi)
    vmovdqa %ymm1, -0x0060(%rdi)
    vmovdqa %ymm1, -0x0040(%rdi)
    vmovdqa %ymm1, -0x0020(%rdi)

    leaq    0x0100(%rdi), %rdi                      # Advance offset
    ja      .Lwr256b                                # Loop if not done

    vzeroupper
    ret

.Lwr128b:
    subq    $0x80, %rdi
    cmpq    %rdi, %rdx                              # Check room for 128 bytes
    jb      .Lwr64b

    vmovdqa %ymm1, -0x80(%rdi)                      # Write 128 bytes
    vmovdqa %ymm1, -0x60(%rdi)
    vmovdqa %ymm1, -0x40(%rdi)
    vmovdqa %ymm1, -0x20(%rdi)

    je      .Lepi                                   # Done if no more bytes

    leaq    0x80(%rdi), %rdi                        # Advance address

.Lwr64b:
    subq    $0x40, %rdi
    cmpq    %rdi, %rdx                              # Check room for 64 bytes
    jb      .Lwr32b

    vmovdqa %ymm1, -0x40(%rdi)                      # Write 64 bytes
    vmovdqa %ymm1, -0x20(%rdi)

    je      .Lepi                                   # Done if not more bytes

    leaq    0x40(%rdi), %rdi                        # Advance address

.Lwr32b:
    subq    $0x20, %rdi
    cmpq    %rdi, %rdx                              # Check room for 32 bytes
    jb      .Lwrlast32b

    vmovdqa %ymm1, -0x20(%rdi)
    je      .Lepi

    leaq    0x20(%rdi), %rdi

.Lwrlast32b:
    vmovdqu %ymm1, -0x20(%rdx)                      # Write last 32 bytes unaligned

.Lepi:
    vzeroupper
    ret

.Lwr32b_unaligned:
    vpxor   %xmm2, %xmm2, %xmm2                     # Shuffle mask
    vmovd   %esi, %xmm1                             # Insert dword
    vpshufb  %xmm2, %xmm1, %xmm0                    # Broadcast 0th byte

    vmovdqu %xmm0, 0x00(%rax)                       # Write
    vmovdqu %xmm0, 0x10(%rax)
    vzeroupper
    ret

.Lwr16b:
    cmpl    $0x10, %edx                             # Check space for 16 bytes
    jb      .Lwr8b

    vpxor   %xmm2, %xmm2, %xmm2                     # Shuffle mask
    vmovd   %esi, %xmm1                             # Insert dword
    vpshufb %xmm2, %xmm1, %xmm0                     # Broadcast 0th byte

    vmovdqu %xmm0, (%rax)                           # Write xmmword

    je      .Lepi                                   # Done if no more bytes

    vmovdqu %xmm0, -0x10(%rax, %rdx)                # Write final xmmword

    vzeroupper
    ret

.Lwr8b:
    cmpl    $0x08, %edx                             # Check space for 8 bytes
    jb      .Lwr4b

    vpxor   %xmm2, %xmm2, %xmm2                     # Shuffle mask
    vmovd   %esi, %xmm1                             # Insert dword
    vpshufb %xmm2, %xmm1, %xmm0                     # Broadcast 0th byte

    vmovq   %xmm0, (%rax)                           # Write first qword

    je      .Lepi                                   # Done if no more bytes

    vmovq   %xmm0, -0x08(%rax, %rdx)                # Write final qword

    vzeroupper
    ret

.Lwr4b:
    cmpl    $0x04, %edx                             # Check space for 4 bytes
    jb      .Lwr2b

    vpxor   %xmm2, %xmm2, %xmm2                     # Shuffle mask
    vmovd   %esi, %xmm1                             # Insert dword
    vpshufb %xmm2, %xmm1, %xmm0                     # Broadcast 0th byte

    vmovd   %xmm0, (%rax)                           # Write first dword

    je      .Lepi                                   # Done if no more bytes

    vmovd   %xmm0, -0x04(%rax, %rdx)                # Write final dword

    vzeroupper
    ret

.Lwr2b:
    cmpl    $0x02, %edx                             # Check space for 2 bytes
    jb      .Lwr1b

    movb    %sil, 0x00(%rax)
    movb    %sil, 0x01(%rax)

    leaq    0x02(%rax), %rdi

    jne      .Lwr1b

    ret

.Lwr1b:
    movb    %sil, (%rdi)
    ret
