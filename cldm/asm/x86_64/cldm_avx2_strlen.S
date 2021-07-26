    .section .text

    #include "asm/system.h"

.globl cldm_avx2_strlen
# AVX2-accelerated strlen
# Params:
#   %rdi: Address of string
# Return:
#   rax: Length of string
cldm_avx2_strlen:
    vpxor   %xmm15, %xmm15, %xmm15

    mov     %edi, %ecx
    andl    $PGSIZE - 1, %ecx                       # Mask out page offset
    cmpl    $PGSIZE - 0x20, %ecx
    ja      .Lpgx16b

    vmovdqu (%rdi), %ymm0                           # Single unaligned read
    vpcmpeqb    %ymm15, %ymm0, %ymm4
    vptest  %ymm4, %ymm4
    jz  .L32balign

    vpmovmskb   %ymm4, %ecx                         # Extract bitmask
    tzcntl  %ecx, %eax                              # Index of null byte in ymmword
    vzeroupper
    ret

.L32balign:
    leaq    0x20(%rdi), %rax                        # Align next read to 32 byte boundary
    andq    $-0x20, %rax
    subq    %rdi, %rax

    .align 16
.Lcmp128b_pgchk:
    leaq    (%rdi, %rax), %rcx
    andl    $PGSIZE - 1, %ecx
    cmpl    $PGSIZE - 0x80, %ecx                    # Check whether reading 4 ymmwords would cross page boundary
    ja      .Lpgx32b

.Lcmp128b:
    vmovdqa 0x00(%rdi, %rax), %ymm0                 # Load and compare 4 ymmwords
    vpcmpeqb    %ymm15, %ymm0, %ymm4
    vmovdqa 0x20(%rdi, %rax), %ymm1
    vpcmpeqb    %ymm15, %ymm1, %ymm5
    vmovdqa 0x40(%rdi, %rax), %ymm2
    vpcmpeqb    %ymm15, %ymm2, %ymm6
    vmovdqa 0x60(%rdi, %rax), %ymm3
    vpcmpeqb    %ymm15, %ymm3, %ymm7

    vpor    %ymm4, %ymm5, %ymm8                     # Reduce
    vpor    %ymm6, %ymm7, %ymm9
    vpor    %ymm8, %ymm9, %ymm10

    leaq    0x80(%rax), %rax                        # Advance

    vptest  %ymm10, %ymm10                          # Set ZF if null was not encountered
    jz      .Lcmp128b_pgchk

    leaq    -0x80(%rax), %rax
    movl    $0x20, %ecx                             # Step size
    xorl    %edx, %edx                              # For conditionally zeroing step

    vptest  %ymm4, %ymm4                            # Set ZF if no null in first ymmword
    cmovnzl %edx, %ecx                              # Conditionally zero step
    leaq    (%rax, %rcx), %rax

    vptest  %ymm5, %ymm5                            # Set ZF if no null in second ymmword
    cmovnzl %edx, %ecx                              # Conditionally zero step
    leaq    (%rax, %rcx), %rax

    vptest  %ymm6, %ymm6                            # Set ZF if no null in third ymmword
    cmovnzl %edx, %ecx                              # Conditionally zero step
    leaq    (%rax, %rcx), %rax

    vmovdqa (%rdi, %rax), %ymm0                     # Load ymmword with first null byte
    vpcmpeqb    %ymm15, %ymm0, %ymm4                # Compare against null
    vpmovmskb   %ymm4, %ecx                         # Extract bitmask
    tzcntl  %ecx, %edx                              # Index of null byte in ymmword
    leaq    (%rax, %rdx), %rax                      # Return value
    vzeroupper
    ret

.Lpgx32b:
    vmovdqa (%rdi, %rax), %ymm0                     # Load and compare single ymmword
    vpcmpeqb    %ymm15, %ymm0, %ymm4
    vptest  %ymm4, %ymm4                            # Check if null byte in ymmword
    jz      .Lpgx32b_next

    vpmovmskb   %ymm4, %esi                         # Extract bitmask
    tzcntl  %esi, %edx                              # Index of null byte in ymmword
    leaq    (%rax, %rdx), %rax                      # Return value
    vzeroupper
    ret

.Lpgx32b_next:
    leaq    0x20(%rax), %rax                        # Advance offset
    leaq    (%rdi, %rax), %rcx                      # Address of next load
    andl    $PGSIZE - 1, %ecx                       # Check for page alignment
    testl   %ecx, %ecx
    jz      .Lcmp128b
    jmp     .Lpgx32b

.Lpgx16b:
    xorl    %eax, %eax                              # Length

    cmpl    $PGSIZE - 0x10, %ecx                    # Check whether reading xmmword would cross boundary
    ja      .Lpgx8b

    vmovdqu (%rdi), %xmm0                           # Load and compare single xmmword
    vpcmpeqb    %xmm15, %xmm0, %xmm4
    vptest  %xmm4, %xmm4                            # Check for null
    jz      .Lpgx16b_next

    vpmovmskb   %xmm4, %esi                         # Bitmask
    tzcntl  %esi, %edx
    leal    (%eax, %edx), %eax
    vzeroupper
    ret

.Lpgx16b_next:
    leal    0x10(%eax), %eax                        # Advance

.Lpgx8b:
    leaq    (%rdi, %rax), %rcx                      # Address of next load
    andl    $PGSIZE - 1, %ecx                       # Mask out page offset
    testl   %ecx, %ecx                              # Check if aligned to boundary
    jz      .Lcmp128b
    cmpl    $PGSIZE - 0x08, %ecx                    # Check if reading qword would cross boundary
    ja      .Lpgx4b

    vmovq   (%rdi, %rax), %xmm0                     # Load qword and compare to null
    vpcmpeqb    %xmm15, %xmm0, %xmm4
    vpslldq $0x08, %xmm4, %xmm7                     # Shift out zeroed lanes
    vptest  %xmm7, %xmm7                            # Check for null
    jz      .Lpgx8b_next

    vpmovmskb   %xmm7, %esi                         # Extract bitmask
    shrl    $0x08, %esi                             # Undo packed right shift
    tzcntl  %esi, %edx
    leal    (%eax, %edx), %eax
    vzeroupper
    ret

.Lpgx8b_next:
    leal    0x08(%eax), %eax

.Lpgx4b:
    leaq    (%rdi, %rax), %rcx                      # Address of next load
    andl    $PGSIZE - 1, %ecx                       # Mask out page offset
    testl   %ecx, %ecx                              # Check if aligned to boundary
    jz      .Lcmp128b
    cmpl    $PGSIZE - 0x04, %ecx                    # Check whether reading dword would cross boundary
    ja      .Lpgx2b

    vmovd   (%rdi, %rax), %xmm0                     # Load dword and compare to null
    vpcmpeqb    %xmm15, %xmm0, %xmm4
    vpslldq $0x0c, %xmm4, %xmm7                     # Shift out zeroed lanes
    vptest  %xmm7,  %xmm7                           # Check for null
    jz      .Lpgx4b_next

    vpmovmskb   %xmm7, %esi                         # Extract bitmask
    shrl    $0x0c, %esi                             # Undo packed right shift
    tzcntl  %esi, %edx
    leal    (%eax, %edx), %eax
    vzeroupper
    ret

.Lpgx4b_next:
    leal    0x04(%eax), %eax

.Lpgx2b:
    leaq    (%rdi, %rax), %rcx                      # Address of next load
    andl    $PGSIZE - 1, %ecx                       # Mask out page offset
    testl   %ecx, %ecx                              # Check if aligned to boundary
    jz      .Lcmp128b
    cmpl    $PGSIZE - 0x02, %ecx                    # Check if reading word would cross boundary
    ja      .Lpgx1b

    movl    $0x01, %ecx                             # Step size
    xorl    %esi, %esi                              # For conditionally zeroing step size

    movzxw  (%rdi, %rax), %edx                      # Load word
    testl   $0xff, %edx
    cmovzl  %esi, %ecx                              # Zero step if first byte is null
    leal    (%eax, %ecx), %eax

    testl   $0xff00, %edx
    cmovzl  %esi, %ecx                              # Zero step if second byte is null
    leal    (%eax, %ecx), %eax

    testl   %ecx, %ecx                              # If step is zero, at least one null byte was encountered
    jz      .Lepi

.Lpgx1b:
    leaq    (%rdi, %rax), %rcx                      # Address of next load
    testl   $0x01, %ecx                             # Check whether aligned to boundary
    jz      .Lcmp128b

    cmpb    $0x00, (%rdi, %rax)                     # Check for null
    je      .Lepi

    leal    0x01(%eax), %eax
    jmp     .Lcmp128b

.Lepi:
    vzeroupper
    ret
