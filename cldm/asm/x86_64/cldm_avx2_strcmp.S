    .section .text

    #include "asm/system.h"

.macro  pgjmp
    leaq    (%rdi, %rcx), %rax                      # Address of next read from rdi
    leaq    (%rsi, %rcx), %rdx                      # Address of next read from rsi
    orl     %edx, %eax                              # Estimate position in page
    subl    $PGSIZE, %eax
    negl    %eax
    tzcntl  %eax, %edx                              # Branch table index
    cmpl    %r9d, %edx                              # Clamp
    cmoval  %r9d, %edx
    jmp     *(%r8, %rdx, 8)                         # Jump to branch
.endm

.globl cldm_avx2_strcmp
# AVX2-accelerated strcmp
# Params:
#   rdi: Address of first string
#   rsi: Address of second string
# Return:
#   eax: 0 if strings are identical
#        < 0 if (%rdi) is less than (%rsi)
#        > 0 if (%rsi) is greater than (%rsi)
cldm_avx2_strcmp:
    xorl    %ecx, %ecx                              # Offset
    vpcmpeqb    %ymm14, %ymm14, %ymm14
    vpxor   %ymm15, %ymm15, %ymm15

    movl    %edi, %eax                              # Compute esitmate of position in current page
    orl     %esi, %eax
    andl    $PGSIZE - 0x01, %eax
    cmpl    $PGSIZE - 0x20, %eax
    ja      .Lpgx

    vmovdqu (%rdi), %ymm0                           # Load ymmword
    vpcmpeqb    (%rsi), %ymm0, %ymm4                # Compare for equality
    vpcmpeqb    %ymm15, %ymm0, %ymm8                # Check for null
    vpandn  %ymm4, %ymm8, %ymm12                    # Set lane to zero if either differing byte or null
    vptest  %ymm14, %ymm12
    jc      .L32balign

    vpmovmskb   %ymm12, %ecx                        # Extract bitmask
    notl    %ecx                                    # Invert
    tzcntl  %ecx, %edx                              # Offset of interesting byte
    movzxb  (%rdi, %rdx), %eax                      # Load byte from each source
    movzxb  (%rsi, %rdx), %ecx
    subl    %ecx, %eax                              # Compute return value
    vzeroupper
    ret

.L32balign:
    leaq    0x20(%rdi, %rcx), %rcx                  # Align next read from (%rdi) to 32-byte boundary
    andq    $-0x20, %rcx
    subq    %rdi, %rcx

    .align 16
.Lcmp128b:
    leaq    (%rdi, %rcx), %rax                      # Address of next read from rdi
    leaq    (%rsi, %rcx), %rdx                      # Address of next read from rsi
    orl     %edx, %eax
    andl    $PGSIZE - 1, %eax                       # Estimate position in current page
    cmpl    $PGSIZE - 0x80, %eax
    ja      .Lpgx

    vmovdqa 0x00(%rdi, %rcx), %ymm0                 # Load and compare 4 ymmwords
    vpcmpeqb    0x00(%rsi, %rcx), %ymm0, %ymm4
    vmovdqa 0x20(%rdi, %rcx), %ymm1
    vpcmpeqb    0x20(%rsi, %rcx), %ymm1, %ymm5
    vmovdqa 0x40(%rdi, %rcx), %ymm2
    vpcmpeqb    0x40(%rsi, %rcx), %ymm2, %ymm6
    vmovdqa 0x60(%rdi, %rcx), %ymm3
    vpcmpeqb    0x60(%rsi, %rcx), %ymm3, %ymm7

    vpand   %ymm4, %ymm5, %ymm8                     # Reduce
    vpand   %ymm6, %ymm7, %ymm9
    vpand   %ymm8, %ymm9, %ymm13

    vpcmpeqb    %ymm15, %ymm0, %ymm9                # Compare each ymmword against null
    vpcmpeqb    %ymm15, %ymm1, %ymm10
    vpcmpeqb    %ymm15, %ymm2, %ymm11
    vpcmpeqb    %ymm15, %ymm3, %ymm12

    vptest  %ymm14, %ymm13                          # Check for differing bytes
    jnc     .Ldiff128b

    vpor    %ymm9, %ymm10, %ymm8                    # Reduce null results
    vpor    %ymm11, %ymm12, %ymm13
    vpor    %ymm8, %ymm13, %ymm8

    leaq    0x80(%rcx), %rcx                        # Advance offset

    vptest  %ymm8, %ymm8                            # Clear zero flag if null byte was encountered
    jz      .Lcmp128b

    xorl    %eax, %eax                              # Return value
    vzeroupper
    ret

.Ldiff128b:
    movl    $0x20, %eax                             # Step size
    xorl    %edx, %edx                              # For conditionally zeroing step

    vpandn  %ymm4, %ymm9, %ymm4                     # Set lane to all zeroes if either null or difference
    vpandn  %ymm5, %ymm10, %ymm5
    vpandn  %ymm6, %ymm11, %ymm6
    vpandn  %ymm7, %ymm12, %ymm7

    vptest  %ymm14, %ymm4                           # Clear carry if first ymmword contains interesting byte
    cmovncl %edx, %eax                              # Conditionally zero step
    leaq    (%rcx, %rax), %rcx                      # Advance offset

    vptest  %ymm14, %ymm5                           # Clear carry if second ymmword contains interesting byte
    cmovncl %edx, %eax                              # Conditionally zero step
    leaq    (%rcx, %rax), %rcx                      # Advance offset

    vptest  %ymm14, %ymm6                           # Clear carry if third ymmword contains interesting byte
    cmovncl %edx, %eax                              # Conditionally zero step
    leaq    (%rcx, %rax), %rcx                      # Advance offset

    vmovdqa (%rdi, %rcx), %ymm0                     # Load offending ymmword from first source
    vpcmpeqb    (%rsi, %rcx), %ymm0, %ymm4          # Compare against second source
    vpcmpeqb    %ymm15, %ymm0, %ymm8                # Compare against null
    vpandn  %ymm4, %ymm8, %ymm12                    # Reduce
    vpmovmskb   %ymm12, %edx                        # Extract bitmask
    notl    %edx                                    # Invert
    tzcntl  %edx, %eax                              # Index of first difference or null in ymmword
    leaq    (%rcx, %rax), %rcx                      # Advance offset

    movzxb  (%rdi, %rcx), %eax                      # Load offending byte from each source
    movzxb  (%rsi, %rcx), %edx
    subl    %edx, %eax                              # Compute return value
    vzeroupper
    ret

.Lpgx:
    leaq    .Laligntbl(%rip), %r8                   # Load jump table
    movl    $0x07, %r9d                             # For clamping jump offset
    subl    $PGSIZE, %eax
    negl    %eax
    tzcntl  %eax, %edx                              # Branch table index
    cmpl    %r9d, %edx                              # Clamp jump offset
    cmoval  %r9d, %edx
    jmp     *(%r8, %rdx, 8)                         # Jump to branch

.Lpgxb:
    movzxb  (%rdi, %rcx), %eax                      # Load bytes
    movzxb  (%rsi, %rcx), %edx
    subl    %edx, %eax                              # Compare
    jnz     .Lepi

    addl    %edx, %eax                              # Check for null
    jz      .Lepi_eq

    leaq    0x01(%rcx), %rcx

    pgjmp

.Lpgxw:
    movzxb  (%rdi, %rcx), %eax                      # Load first byte from each source
    movzxb  (%rsi, %rcx), %edx

    subl    %edx, %eax                              # Check for difference
    jnz     .Lepi

    addl    %edx, %eax                              # Check for null
    jz      .Lepi_eq

    movzxb  0x01(%rdi, %rcx), %eax                  # Load second byte
    movzxb  0x01(%rsi, %rcx), %edx

    subl    %edx, %eax                              # Check for difference
    jnz     .Lepi

    addl    %edx, %eax                              # Check for null
    jz      .Lepi_eq

    leaq    0x02(%rcx), %rcx

    pgjmp

.Lpgxdw:
    vmovd   (%rdi, %rcx), %xmm0                     # Load dwords
    vmovd   (%rsi, %rcx), %xmm1
    vpcmpeqb    %xmm0, %xmm1, %xmm2                 # Compare for equality
    vpcmpeqb    %xmm15, %xmm0, %xmm3                # Compare against null

    vptest  %xmm14, %xmm2                           # Check if different
    jnc     .Lpgxdiff

    vpmovmskb   %xmm3, %eax                         # Extract bitmask
    tzcntl  %eax, %edx                              # Index of null byte
    cmpl    $0x04, %edx                             # Done if null byte is within dword
    jb      .Lepi_eq

    leaq    0x04(%rcx), %rcx

    pgjmp

.Lpgxqw:
    vmovq   (%rdi, %rcx), %xmm0                     # Load qwords
    vmovq   (%rsi, %rcx), %xmm1
    vpcmpeqb    %xmm0, %xmm1, %xmm2                 # Compare for equality
    vpcmpeqb    %xmm15, %xmm0, %xmm3                # Compare against null

    vptest  %xmm14, %xmm2                           # Check if different
    jnc     .Lpgxdiff

    vpmovmskb   %xmm3, %eax                         # Extract bitmask
    tzcntl  %eax, %edx                              # Index of null byte
    cmpl    $0x08, %edx                             # Done if null is within qword
    jb      .Lepi_eq

    leaq    0x08(%rcx), %rcx

    pgjmp

.Lpgxxw:
    vmovdqu (%rdi, %rcx), %xmm0                     # Load and compare xmmwords
    vpcmpeqb    (%rsi, %rcx), %xmm0, %xmm2
    vpcmpeqb    %xmm15, %xmm0, %xmm3                # Check for null

    vptest  %xmm14, %xmm2                           # Check for difference
    jnc     .Lpgxdiff

    vptest  %xmm3, %xmm3
    jnz     .Lepi_eq

    leaq    0x10(%rcx), %rcx

    pgjmp

.Lpgxyw:
    vmovdqu (%rdi, %rcx), %ymm0                     # Load and compare ymmwords
    vpcmpeqb    (%rsi, %rcx), %ymm0, %ymm2
    vpcmpeqb    %ymm15, %ymm0, %ymm3                # Check for null

    vptest  %ymm14, %ymm2                           # Check for difference
    jnc     .Lpgxdiff

    vptest  %ymm3, %ymm3
    jnz     .Lepi_eq

    leaq    0x20(%rcx), %rcx

    pgjmp

.Lpgxzw:
    vmovdqu 0x00(%rdi, %rcx), %ymm0                 # Load and compare 2 ymmwords
    vpcmpeqb    0x00(%rsi, %rcx), %ymm0, %ymm4
    vmovdqu 0x20(%rdi, %rcx), %ymm1
    vpcmpeqb    0x20(%rsi, %rcx), %ymm1, %ymm5

    vpand   %ymm4, %ymm5, %ymm13                    # Reduce

    vpcmpeqb    %ymm15, %ymm0, %ymm9                # Compare against null
    vpcmpeqb    %ymm15, %ymm1, %ymm10

    vptest  %ymm14, %ymm13                          # Check for difference
    jnc     .Lpgxzwdiff

    vpor    %ymm9, %ymm10, %ymm8                    # Reduce

    vptest  %ymm8, %ymm8                            # Check if done
    jnz     .Lepi_eq

    leaq    0x40(%rcx), %rcx

    pgjmp

.Lpgxalign:
    vmovdqu (%rdi, %rcx), %ymm0                     # Load ymmword
    vpcmpeqb    (%rsi, %rcx), %ymm0, %ymm4          # Compare for equality
    vpcmpeqb    %ymm15, %ymm0, %ymm8                # Check for null
    vpandn  %ymm4, %ymm8, %ymm12                    # Set lane to zero if either differing byte or null
    vptest  %ymm14, %ymm12
    jc      .L32balign

    vpmovmskb   %ymm12, %edx                        # Extract bitmask
    notl    %edx                                    # Invert
    tzcntl  %edx, %eax                              # Offset of interesting byte
    leaq    (%rcx, %rax), %rcx
    movzxb  (%rdi, %rcx), %eax                      # Load byte from each source
    movzxb  (%rsi, %rcx), %ecx
    subl    %ecx, %eax                              # Compute return value
    vzeroupper
    ret

.Lpgxdiff:
    vpmovmskb   %ymm2, %eax                         # Extract bitmask for difference
    notl    %eax                                    # Invert
    tzcntl  %eax, %edx                              # Index of differing byte
    leaq    (%rcx, %rdx), %rcx                      # Offset from start of differing byte

    vpmovmskb   %ymm3, %eax                         # Extract bitmask for null
    tzcntl  %eax, %r8d                              # Index of null
    cmpl    %edx, %r8d                              # Done if null before differing byte
    jb      .Lepi_eq

    movzxb  (%rdi, %rcx), %eax                      # Load differing bytes
    movzxb  (%rsi, %rcx), %edx
    subl    %edx, %eax                              # Compute return value
    vzeroupper
    ret

.Lpgxzwdiff:
    movl    $0x20, %eax                             # Step size
    xorl    %edx, %edx                              # For conditionally zeroing step

    vpandn  %ymm4, %ymm9, %ymm4                     # Set lane to all zeroes if either null or difference
    vpandn  %ymm5, %ymm10, %ymm5

    vptest  %ymm14, %ymm4                           # Clear carry if first ymmword contains interesting byte
    cmovncl %edx, %eax                              # Conditionally zero step
    leaq    (%rcx, %rax), %rcx                      # Advance offset

    vptest  %ymm14, %ymm5                           # Clear carry if second ymmword contains interesting byte
    cmovncl %edx, %eax                              # Conditionally zero step
    leaq    (%rcx, %rax), %rcx                      # Advance offset

    vmovdqa (%rdi, %rcx), %ymm0                     # Load offending ymmword from first source
    vpcmpeqb    (%rsi, %rcx), %ymm0, %ymm4          # Compare against second source
    vpcmpeqb    %ymm15, %ymm0, %ymm8                # Compare against null
    vpandn  %ymm4, %ymm8, %ymm12                    # Reduce
    vpmovmskb   %ymm12, %edx                        # Extract bitmask
    notl    %edx                                    # Invert
    tzcntl  %edx, %eax                              # Index of first difference or null in ymmword
    leaq    (%rcx, %rax), %rcx                      # Advance offset

    movzxb  (%rdi, %rcx), %eax                      # Load offending byte from each source
    movzxb  (%rsi, %rcx), %edx
    subl    %edx, %eax                              # Compute return value
    vzeroupper
    ret

.Lepi_eq:
    xorl    %eax, %eax
.Lepi:
    vzeroupper
    ret

    .section .data
    .align 8
.Laligntbl:
    .quad   .Lpgxb
    .quad   .Lpgxw
    .quad   .Lpgxdw
    .quad   .Lpgxqw
    .quad   .Lpgxxw
    .quad   .Lpgxyw
    .quad   .Lpgxzw
    .quad   .Lpgxalign

