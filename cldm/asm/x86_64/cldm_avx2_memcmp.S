    .section .text

.macro diffprlg offset
    subq    \offset, %rcx                           # Offset of first 256-byte block with differing data
    movl    $0x20, %r8d                             # Size of ymmword
    xorl    %r9d, %r9d                              # All zeroes for conditionally disabling offset
.endm

.macro  cmask ymmword
    vptest  %ymm0, \ymmword                         # Clear carry if difference detected in \ymmword
    cmovncl %r9d, %r8d                              # Clear offset if difference detected
    leaq    (%rcx, %r8), %rcx                       # Advance offset if not difference has been detected
.endm

.macro  rvcomp
    vmovdqa (%rdi, %rcx), %ymm1                     # Load and compare first differing ymmwords
    vpcmpeqb    (%rsi, %rcx), %ymm1, %ymm2

    vpmovmskb   %ymm2, %r8d                         # Extract bitmask
    notl    %r8d                                    # Difference corresponds to a 0, invert
    tzcntl  %r8d, %edx                              # Offset of differing byte in ymmword
    leaq    (%rcx, %rdx), %rcx                      # Offset relative source addresses
    movzxb  (%rdi, %rcx), %eax                      # Load differing bytes
    movzxb  (%rsi, %rcx), %edx
    subl    %edx, %eax                              # Compute return value
.endm

.globl cldm_avx2_memcmp
# AVX2-accelerated memcmp
# Params:
#   rdi: Address of first memory area
#   rsi: Address of second memory area
#   rdx: Number of bytes to compare
# Return:
#   eax: 0 if the memory areas are the same,
#        < 0 if area (%rdi) is less than (%rsi),
#        > 0 if area (%rdi) is greater than (%rsi)
cldm_avx2_memcmp:
    vpcmpeqb    %ymm0, %ymm0, %ymm0                 # All ones

    cmpq    $0x20, %rdx
    jb      .Lc16b
    je      .Lc32b_unaligned

    movl    $0x20, %ecx                             # Address offset

    tzcntq  %rdi, %r8                               # Least significant set bit
    cmpl    $0x05, %r8d                             # Check if aligned
    jnb     .Lc256b_prlg

    vmovdqu (%rdi), %ymm1                           # Load first ymmword
    vpcmpeqb    (%rsi), %ymm1, %ymm2                # Compare first ymmwords

    vptest  %ymm0, %ymm2                            # Clear carry if ymmwords differ
    jnc     .Lc32bdiff

    leaq    0x20(%rdi, %rcx), %rcx                  # Compute offset for end of first 256-byte block
    andq    $-0x20, %rcx
    subq    %rdi, %rcx

.Lc256b_prlg:
    leaq    0xe0(%rcx), %rcx

    .align 16
.Lc256b:
    cmpq    %rcx, %rdx                              # Check room for 256 bytes
    jb      .Lc128b

    vmovdqa -0x0100(%rdi, %rcx), %ymm1              # Load 8 ymmwords from each source and compare
    vpcmpeqb    -0x0100(%rsi, %rcx), %ymm1, %ymm2

    vmovdqa -0x00e0(%rdi, %rcx), %ymm1
    vpcmpeqb    -0x00e0(%rsi, %rcx), %ymm1, %ymm3

    vmovdqa -0x00c0(%rdi, %rcx), %ymm1
    vpcmpeqb    -0x00c0(%rsi, %rcx), %ymm1, %ymm4

    vmovdqa -0x00a0(%rdi, %rcx), %ymm1
    vpcmpeqb    -0x00a0(%rsi, %rcx), %ymm1, %ymm5

    vmovdqa -0x0080(%rdi, %rcx), %ymm1
    vpcmpeqb    -0x0080(%rsi, %rcx), %ymm1, %ymm6

    vmovdqa -0x0060(%rdi, %rcx), %ymm1
    vpcmpeqb    -0x0060(%rsi, %rcx), %ymm1, %ymm7

    vmovdqa -0x0040(%rdi, %rcx), %ymm1
    vpcmpeqb    -0x0040(%rsi, %rcx), %ymm1, %ymm8

    vmovdqa -0x0020(%rdi, %rcx), %ymm1
    vpcmpeqb    -0x0020(%rsi, %rcx), %ymm1, %ymm9

    vpand   %ymm2, %ymm3, %ymm10                    # Reduce results
    vpand   %ymm4, %ymm5, %ymm11
    vpand   %ymm6, %ymm7, %ymm12
    vpand   %ymm8, %ymm9, %ymm13
    vpand   %ymm10, %ymm11, %ymm14
    vpand   %ymm12, %ymm13, %ymm15
    vpand   %ymm14, %ymm15, %ymm15

    vptest  %ymm0, %ymm15                           # Clear carry if differences were encountered
    jnc     .Lc256bdiff

    cmpq    %rcx, %rdx                              # Check for remaining bytes
    leaq    0x0100(%rcx), %rcx                      # Advance offset
    ja      .Lc256b
    xorl    %eax, %eax
    vzeroupper
    ret

.Lc128b:
    subq    $0x80, %rcx
    cmpq    %rcx, %rdx                              # Check room for 128 bytes
    jb      .Lc64b

    vmovdqa -0x80(%rdi, %rcx), %ymm1                # Load 4 ymmwords from each source and compare
    vpcmpeqb    -0x80(%rsi, %rcx), %ymm1, %ymm2

    vmovdqa -0x60(%rdi, %rcx), %ymm1
    vpcmpeqb    -0x60(%rsi, %rcx), %ymm1, %ymm3

    vmovdqa -0x40(%rdi, %rcx), %ymm1
    vpcmpeqb    -0x40(%rsi, %rcx), %ymm1, %ymm4

    vmovdqa -0x20(%rdi, %rcx), %ymm1
    vpcmpeqb    -0x20(%rsi, %rcx), %ymm1, %ymm5

    vpand   %ymm2, %ymm3, %ymm6                     # Reduce
    vpand   %ymm4, %ymm5, %ymm7
    vpand   %ymm6, %ymm7, %ymm8

    vptest  %ymm0, %ymm8                            # Clear carry if differences were encountered
    jnc     .Lc128bdiff

    cmpq    %rcx, %rdx                              # Check if done
    jna     .Lepi

    leaq    0x80(%rcx), %rcx

.Lc64b:
    subq    $0x40, %rcx
    cmpq    %rcx, %rdx                              # Check room for 64 bytes
    jb      .Lc32b

    vmovdqa -0x40(%rdi, %rcx), %ymm1                # Load 2 ymmwords from each source and compare
    vpcmpeqb    -0x40(%rsi, %rcx), %ymm1, %ymm2

    vmovdqa -0x20(%rdi, %rcx), %ymm1
    vpcmpeqb    -0x20(%rsi, %rcx), %ymm1, %ymm3

    vpand   %ymm2, %ymm3, %ymm4                     # Reduce

    vptest  %ymm0, %ymm4                            # Clear carry if differences were encountered
    jnc     .Lc64bdiff

    cmpq    %rcx, %rdx                              # Check if done
    jna     .Lepi

    leaq    0x40(%rcx), %rcx

.Lc32b:
    subq    $0x20, %rcx
    cmpq    %rcx, %rdx                              # Check room for 32 bytes
    jb      .Lclast32b

    vmovdqa -0x20(%rdi, %rcx), %ymm1                # Load and compare single ymmword from each source
    vpcmpeqb    -0x20(%rsi, %rcx), %ymm1, %ymm2

    vptest  %ymm0, %ymm2
    jnc     .Lc32bdiff

    cmpq    %rcx, %rdx                              # Check if done
    jna     .Lepi

    leaq    0x20(%rcx), %rcx

.Lclast32b:
    vmovdqu -0x20(%rdi, %rdx), %ymm1                # Load and compare
    vpcmpeqb    -0x20(%rsi, %rdx), %ymm1, %ymm2

    vptest  %ymm0, %ymm2                            # Clear carry if differences were encountered
    jc      .Lepi

    vpmovmskb   %ymm2, %ecx                         # Extract bitmask
    notl    %ecx                                    # Invert
    tzcntl  %ecx, %r8d                              # Vector index of offending byte
    leaq    -0x20(%rdx, %r8), %rdx                  # Compute offset

    movzxb  (%rdi, %rdx), %eax                      # Load byte from first source
    movzxb  (%rsi, %rdx), %ecx                      # Byte from second source
    subl    %ecx, %eax                              # Return value
    vzeroupper
    ret

.Lc256bdiff:
    diffprlg    $0x0100                             # Set up initial offset and masks

    cmask   %ymm2                                   # Compute offset of first differing ymmword
    cmask   %ymm3
    cmask   %ymm4
    cmask   %ymm5
    cmask   %ymm6
    cmask   %ymm7
    cmask   %ymm8

    rvcomp                                          # Compute return value

    vzeroupper
    ret

.Lc128bdiff:
    diffprlg    $0x80                               # Set up initial offset and masks

    cmask   %ymm2                                   # Compute offset of first differing ymmword
    cmask   %ymm3
    cmask   %ymm4

    rvcomp                                          # Compute return value

    vzeroupper
    ret

.Lc64bdiff:
    diffprlg    $0x40                               # Set up initial offset and masks

    cmask %ymm2                                     # Compute offset of first differing ymmword

    rvcomp                                          # Compute return value

    vzeroupper
    ret

.Lc32bdiff:
    vpmovmskb   %ymm2, %r8d                         # Extract bitmask
    notl    %r8d                                    # Difference corresponds to a 0, invert
    tzcntl  %r8d, %edx                              # Offset of differing byte in ymmword
    leaq    -0x20(%rcx, %rdx), %rcx                 # Offset relative source addresses
    movzxb  (%rdi, %rcx), %eax                      # Load differing bytes
    movzxb  (%rsi, %rcx), %edx
    subl    %edx, %eax                              # Compute return value

    vzeroupper
    ret

.Lc32b_unaligned:
    vmovdqu (%rdi), %ymm1                           # Load and compare first ymmword
    vpcmpeqb    (%rsi), %ymm1, %ymm2
    vptest  %ymm0, %ymm2                            # Set carry if ymmwords are equal
    jnc     .Lc32bdiff_unaligned

    xorl    %eax, %eax
    vzeroupper
    ret

.Lc32bdiff_unaligned:
    vpmovmskb   %ymm2, %ecx                         # Extract bitmask
    notl    %ecx                                    # Difference corresponds to 0, invert
    tzcntl  %ecx, %edx                              # Offset of differing byte
    movzxb  (%rdi, %rdx), %eax                      # Load bytes
    movzxb  (%rsi, %rdx), %ecx
    subl    %ecx, %eax                              # Compute return value

    vzeroupper
    ret

.Lc16b:
    xorl    %r8d, %r8d
    cmpl    $0x10, %edx
    jb      .Lc8b

    vmovdqu (%rdi), %xmm1                           # Load and compare first xmmword
    vpcmpeqb    (%rsi), %xmm1, %xmm2

    leal    0x10(%r8d), %r8d
    vptest  %xmm0, %xmm2                            # Set carry if equal
    jc      .Lc8b

    vpmovmskb %xmm2, %ecx                           # Extract bitmask
    notl    %ecx                                    # Invert
    tzcntl  %ecx, %r8d                              # Offset of offending byte
    movzxb  (%rdi, %r8), %eax                       # Compute return value
    movzxb  (%rsi, %r8), %ecx
    subl    %ecx, %eax
    vzeroupper
    ret

.Lc8b:
    cmpl    %r8d, %edx                              # Check if done
    je      .Lepi

    leaq    (%rdi, %rdx), %rdi                      # Set up for using negative offsets to avoid extra cmpl in loop
    leaq    (%rsi, %rdx), %rsi
    negq    %rdx                                    # Negate offset
    movq    $-1, %r8                                # For early exit
.Lc1b:
    movzxb  (%rdi, %rdx), %eax                      # Compare bytes
    movzxb  (%rsi, %rdx), %ecx

    subl    %ecx, %eax                              # Compute potential return value
    cmovnzq %r8, %rdx                               # If bytes no equal, make next addl set ZF

    addq    $1, %rdx                                # Advance offset
    jnz     .Lc1b

    vzeroupper
    ret

.Lepi:
    xorl    %eax, %eax
    vzeroupper
    ret
