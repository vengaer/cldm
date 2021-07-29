    .section .text

    #include "asm/system.h"

.macro  vchk chwd, nullwd, offset
    vpmovmskb   \chwd, %r8d
    vpmovmskb   \nullwd, %r9d
    tzcntl  %r9d, %ecx                              # Index of null byte in ymmword
    jc      1f                                      # Mask only if null byte is found

    movl    $0x01, %r9d                             # Construct bitmask
    shll    %cl, %r9d
    leal    -0x01(%r9d, %r9d), %ecx
    andl    %ecx, %r8d                              # Mask out potential matches after null byte
1:
    lzcntl  %r8d, %r9d                              # Index of last matching character relative msb
    notq    %r9                                     # Multiply by -1 and subtract 1
    leaq    0x20(%r9, %rdx), %r8
    leaq    \offset(%rdi, %r8), %r9
    cmovncq %r9, %rax                               # Store address if there was a match

    vptest  \nullwd, \nullwd
    jnz     .Lepi
.endm

.globl cldm_avx2_strrchr
# AVX2-accelerated strrchr
# Params:
#   rdi: Address of string
#   esi: The byte to search for
# Return:
#   rax: Address of last %esi found in (%rdi),
#        or 0 if none is found
cldm_avx2_strrchr:
    xorl    %eax, %eax                              # Return value
    xorl    %edx, %edx                              # Offset
    vpxor   %ymm15, %ymm15, %ymm15

    vpxor   %xmm2, %xmm2, %xmm2                     # Shuffle mask
    vmovd   %esi, %xmm1                             # Insert dword
    vpshufb %xmm2, %xmm1, %xmm0                     # Broadcast first byte
    vinserti128 $0x01, %xmm0, %ymm0, %ymm14         # Copy low xmmword to high

    movl    %edi, %ecx                              # Compute position in current page
    andl    $PGSIZE - 1, %ecx
    cmpl    $PGSIZE - 0x20, %ecx
    ja      .Lpgx16b

    vmovdqu (%rdi), %ymm0                           # Load first ymmword
    vpcmpeqb    %ymm14, %ymm0, %ymm4                # Compare against char
    vpcmpeqb    %ymm15, %ymm0, %ymm8                # Compare against null

    vpor    %ymm4, %ymm8, %ymm12
    vptest  %ymm12, %ymm12
    jz      .L32balign

    vchk    %ymm4, %ymm8, 0x00

.L32balign:
    leaq    0x20(%rdi), %rdx                        # Align next read to 32 byte boundary
    andq    $-0x20, %rdx
    subq    %rdi, %rdx

    .align 16
.Lchk128b_prlg:
    leaq    (%rdi, %rdx), %rcx                      # Check if reading 128 bytes would cross page boundary
    andl    $PGSIZE - 1, %ecx
    cmpl    $PGSIZE - 0x80, %ecx
    ja      .Lpgx32b

.Lchk128b:
    vmovdqa 0x00(%rdi, %rdx), %ymm0                 # Load ymmword
    vpcmpeqb    %ymm14, %ymm0, %ymm4                # Compare against char
    vpcmpeqb    %ymm15, %ymm0, %ymm8                # Compare against null
    vmovdqa 0x20(%rdi, %rdx), %ymm1                 # Repeat for remaining 96 bytes
    vpcmpeqb    %ymm14, %ymm1, %ymm5
    vpcmpeqb    %ymm15, %ymm1, %ymm9
    vmovdqa 0x40(%rdi, %rdx), %ymm2
    vpcmpeqb    %ymm14, %ymm2, %ymm6
    vpcmpeqb    %ymm15, %ymm2, %ymm10
    vmovdqa 0x60(%rdi, %rdx), %ymm3
    vpcmpeqb    %ymm14, %ymm3, %ymm7
    vpcmpeqb    %ymm15, %ymm3, %ymm11

    vpor    %ymm4, %ymm8, %ymm0                     # Reduce
    vpor    %ymm5, %ymm9, %ymm1
    vpor    %ymm6, %ymm10, %ymm2
    vpor    %ymm7, %ymm11, %ymm3
    vpor    %ymm0 , %ymm1, %ymm12
    vpor    %ymm2, %ymm12, %ymm12
    vpor    %ymm3, %ymm12, %ymm12

    leaq    0x80(%rdx), %rdx

    vptest  %ymm12, %ymm12                          # Check if either null or matching byte was encountered
    jz      .Lchk128b_prlg

    vptest  %ymm0, %ymm0                           # Check whether interesting byte in first ymmword
    jz      .Lchk128b_ymmwd1

    vchk    %ymm4, %ymm8, -0x80

.Lchk128b_ymmwd1:
    vptest  %ymm1, %ymm1                            # Check whether interesting byte in second ymmword
    jz      .Lchk128b_ymmwd2

    vchk    %ymm5, %ymm9, -0x60

.Lchk128b_ymmwd2:
    vptest  %ymm2, %ymm2                            # Check whether interesting byte in third ymmword
    jz      .Lchk128b_ymmwd3

    vchk    %ymm6, %ymm10, -0x40

.Lchk128b_ymmwd3:
    vptest  %ymm3, %ymm3                            # Check whether interesting byte in fourth ymmword
    jz      .Lchk128b_prlg

    vchk    %ymm7, %ymm11, -0x20
    jmp     .Lchk128b_prlg

.Lpgx32b:
    vmovdqa (%rdi, %rdx), %ymm0                     # Process single ymmword at a time
    vpcmpeqb    %ymm14, %ymm0, %ymm4                # Compare against char
    vpcmpeqb    %ymm15, %ymm0, %ymm8                # Compare against null

    vpor    %ymm4, %ymm8, %ymm0                     # Reduce
    vptest  %ymm0, %ymm0
    jz      .Lpgx32b_boundchk

    vchk    %ymm4, %ymm8, 0x00

.Lpgx32b_boundchk:
    leaq    0x20(%rdx), %rdx                        # Check if on page boundary
    leaq    (%rdi, %rdx), %rcx
    andl    $PGSIZE - 1, %ecx
    testl   %ecx, %ecx
    jz      .Lchk128b                               # On boundary, safe to read 4 ymmwords again
    jmp     .Lpgx32b

.Lpgx16b:
    cmpl    $PGSIZE - 0x10, %ecx                    # Check if reading xmmword would cross boundary
    ja      .Lpgx8b

    vmovdqu (%rdi), %xmm0                           # Load xmmword
    vpcmpeqb    %xmm14, %xmm0, %xmm4                # Compare against char
    vpcmpeqb    %xmm15, %xmm0, %xmm8                # Compare against null

    leaq    0x10(%rdi), %rdx                        # Align next read to 16 byte boundary
    andq    $-0x10, %rdx
    subq    %rdi, %rdx

    vpor    %xmm8, %xmm4, %xmm0                     # Reduce

    movl    %ecx, %r11d                             # Store page position

    vptest  %xmm0, %xmm0                            # Check for null or matching bytes
    jz      .Lpgx16b_aligned

    vpmovmskb   %xmm4, %r8d                         # Extract char mask
    vpmovmskb   %xmm8, %r9d                         # Extract null mask
    tzcntl  %r9d, %ecx                              # Index of first null byte
    jc      .Lpgx16b_mskd                           # If no null byte, no need to mask

    movl    $0x01, %r9d                             # Mask out char matches after null byte
    shll    %cl, %r9d
    leal    -0x01(%r9d, %r9d), %ecx                 # Construct bitmask
    andl    %ecx, %r8d                              # Apply mask

.Lpgx16b_mskd:
    shll    $0x10, %r8d                             # Shift out bits corresponding to zeroed lanes
    lzcntl  %r8d, %r9d                              # Index of last matching character relative msb
    notq    %r9                                     # Multiply by -1 and subtract 1
    leaq    0x10(%rdi, %r9), %r8                    # Address of last match
    cmovncq %r8, %rax                               # Store if there was a matching char

    vptest  %xmm8, %xmm8
    jnz     .Lepi

.Lpgx16b_aligned:
    leal    (%r11d, %edx), %r11d                    # Check if aligned to page boundary
    cmpl    $PGSIZE, %r11d
    je      .Lchk128b

    vmovdqa (%rdi, %rdx), %xmm0
    vpcmpeqb    %xmm14, %xmm0, %xmm4                # Compare against char
    vpcmpeqb    %xmm15, %xmm0, %xmm8                # Compare against null

    leaq    0x10(%rdx), %rdx                        # Advance offset

    vpor    %xmm4, %xmm8, %xmm0                     # Reduce

    vptest  %xmm0, %xmm0                            # Check for null or matching bytes
    jz      .Lchk128b

    vpmovmskb   %xmm4, %r8d                         # Extract char mask
    vpmovmskb   %xmm8, %r9d                         # Null mask

    tzcntl  %r9d, %ecx                              # Index of first null byte
    jc      .Lpgx16b_aligned_mskd                   # No need to mask if there is no null byte

    movl    $0x01, %r9d                             # Mask out matches after null byte
    shll    %cl, %r9d
    leal    -0x01(%r9d, %r9d), %ecx                 # Construct bitmask
    andl    %ecx, %r8d                              # Apply mask

.Lpgx16b_aligned_mskd:
    shll    $0x10, %r8d                             # Shift out bits corresponding to zeroed lanes
    lzcntl  %r8d, %r9d                              # Index of last amtching character relative msb
    notq    %r9                                     # Multiply by -1 and subtract 1
    leaq    (%r9, %rdx), %r8                        # Offset of last matching char relative %rdi
    leaq    (%rdi, %r8), %r9                        # Address of last matching char
    cmovncq %r9, %rax                               # Store if there was a matching char

    vptest  %ymm8, %ymm8                            # Check if done
    jnz     .Lepi
    jmp     .Lchk128b

.Lpgx8b:
    xorl    %r10d, %r10d                            # For indicating null byte

    cmpl    $PGSIZE - 0x08, %ecx                    # Check if reading qword would cross boundary
    ja      .Lpgx4b

    vmovq   (%rdi), %xmm0                           # Read qword
    vpcmpeqb    %xmm14, %xmm0, %xmm4                # Compare against char
    vpcmpeqb    %xmm15, %xmm0, %xmm8                # Compare against null

    leaq    0x08(%rdi), %rdx                        # Align next read to 8 byte boundary
    andq    $-0x08, %rdx
    subq    %rdi, %rdx

    vpor    %xmm4, %xmm8, %xmm12                    # Reduce
    vpslldq $0x08, %xmm12, %xmm0                    # Shift out zeroed lanes

    mov     %ecx, %r11d                             # Store position in page

    vptest  %xmm0, %xmm0                            # Clear ZF if interesting byte found
    jz      .Lpgx8b_aligned

    vpmovmskb   %xmm4, %r8d                         # Char mask
    vpmovmskb   %xmm8, %r9d                         # Null mask

    andl    $0xff, %r9d                             # Mask out bits corresponding to zeroed lanes
    tzcntl  %r9d, %ecx                              # Index of null byte in qword
    setncb  %r10b                                   # Indicate presence of null byte
    jc      .Lpgx8b_mskd

    movl    $0x01, %r9d
    shll    %cl, %r9d
    leal    -0x01(%r9d, %r9d), %ecx                 # Construct bitmask
    andl    %ecx, %r8d

.Lpgx8b_mskd:
    shll    $0x18, %r8d                             # Shift out bits corresponding to zeroed lanes
    lzcntl  %r8d, %r9d                              # Index of last matching character relative msb
    notq    %r9                                     # Multiply by -1 and subtract 1
    leaq    0x08(%rdi, %r9), %r8                    # Address of last matching char
    cmovncq %r8, %rax                               # Store if there was a matching char

    testl   %r10d, %r10d                            # Check if null was encountered
    jnz     .Lepi

.Lpgx8b_aligned:
    leal    (%r11d, %edx), %r11d                    # Check if aligned to boundary
    cmpl    $PGSIZE, %r11d
    je      .Lchk128b

    vmovq   (%rdi, %rdx), %xmm0
    vpcmpeqb    %xmm14, %xmm0, %xmm4                # Compare against char
    vpcmpeqb    %xmm15, %xmm0, %xmm8                # Compare against null

    leaq    0x08(%rdx), %rdx                        # Advance offset
    vpor    %xmm4, %xmm8, %xmm12                    # Reduce
    vpslldq $0x08, %xmm12, %xmm0                    # Shift out zeroed lanes
    vptest  %xmm0, %xmm0                            # Clear ZF if interesting byte found
    jz      .Lchk128b

    vpmovmskb   %xmm4, %r8d                         # Char mask
    vpmovmskb   %xmm8, %r9d                         # Null mask

    andl    $0xff, %r9d                             # Mask out bits corresponding to zeroed lanes
    tzcntl  %r9d, %ecx                              # Index of null byte in qword
    setnc   %r10b                                   # Indicate presence of null byte
    jc      .Lpgx8b_aligned_mskd

    movl    $0x01, %r9d                             # Mask out matches after null byte
    shll    %cl, %r9d
    leal    -0x01(%r9d, %r9d), %ecx                 # Construct bitmask
    andl    %ecx, %r8d                              # Apply mask

.Lpgx8b_aligned_mskd:
    shll    $0x18, %r8d                             # Shift out bits corresponding to zeroed lanes
    lzcntl  %r8d, %r9d                              # Index of last matching character relative msb
    notq    %r9                                     # Multiply by -1 and subtract 1
    leaq    (%r9, %rdx), %r8                        # Offset of last matching char relative %rdi
    leaq    (%rdi, %r8), %r9                        # Address of last matching char
    cmovncq %r9, %rax                               # Store if there was a matching char

    testl   %r10d, %r10d                            # Check if null byte was encountered
    jnz     .Lepi

    jmp     .Lchk128b

.Lpgx4b:
    cmpl    $PGSIZE - 0x04, %ecx                    # Check if reading dword would cross boundary
    ja      .Lpgx2b

    vmovd   (%rdi), %xmm0                           # Read dword
    vpcmpeqb    %xmm14, %xmm0, %xmm4                # Compare against char
    vpcmpeqb    %xmm15, %xmm0, %xmm8                # Compare against null

    leaq    0x04(%rdi), %rdx                        # Align next read to 4 byte boundary
    andq    $-0x04, %rdx
    subq    %rdi, %rdx

    vpor    %xmm4, %xmm8, %xmm12                    # Reduce
    vpslldq $0x0c, %xmm12, %xmm0                    # Shift out zeroed lanes

    mov     %ecx, %r11d                             # Store position in page

    vptest  %xmm0, %xmm0                            # Clear ZF if interesting byte found
    jz      .Lpgx4b_aligned

    vpmovmskb   %xmm4, %r8d                         # Char mask
    vpmovmskb   %xmm8, %r9d                         # Null mask

    andl    $0x0f, %r9d                             # Mask out bits corresponding to zeroed lanes
    tzcntl  %r9d, %ecx                              # Index of null byte in dword
    setncb  %r10b                                   # Indicate presence of null byte
    jc      .Lpgx4b_mskd

    movl    $0x01, %r9d
    shll    %cl, %r9d
    leal    -0x01(%r9d, %r9d), %ecx                 # Construct bitmask
    andl    %ecx, %r8d

.Lpgx4b_mskd:
    shll    $0x1c, %r8d                             # Shift out bits corresponding to zeroed lanes
    lzcntl  %r8d, %r9d                              # Index of last matching character relative msb
    notq    %r9                                     # Multiply by -1 and subtract 1
    leaq    0x04(%rdi, %r9), %r8                    # Address of last matching char
    cmovncq %r8, %rax                               # Store if there was a matching char

    testl   %r10d, %r10d                            # Check if null was encountered
    jnz     .Lepi

.Lpgx4b_aligned:
    leal    (%r11d, %edx), %r11d                    # Check if aligned to boundary
    cmpl    $PGSIZE, %r11d
    je      .Lchk128b

    vmovd   (%rdi, %rdx), %xmm0
    vpcmpeqb    %xmm14, %xmm0, %xmm4                # Compare against char
    vpcmpeqb    %xmm15, %xmm0, %xmm8                # Compare against null

    leaq    0x04(%rdx), %rdx                        # Advance offset
    vpor    %xmm4, %xmm8, %xmm12                    # Reduce
    vpslldq $0x0c, %xmm12, %xmm0                    # Shift out zeroed lanes
    vptest  %xmm0, %xmm0                            # Clear ZF if interesting byte found
    jz      .Lchk128b

    vpmovmskb   %xmm4, %r8d                         # Char mask
    vpmovmskb   %xmm8, %r9d                         # Null mask

    andl    $0x0f, %r9d                             # Mask out bits corresponding to zeroed lanes
    tzcntl  %r9d, %ecx                              # Index of null byte in dword
    setnc   %r10b                                   # Indicate presence of null byte
    jc      .Lpgx4b_aligned_mskd

    movl    $0x01, %r9d                             # Mask out matches after null byte
    shll    %cl, %r9d
    leal    -0x01(%r9d, %r9d), %ecx                 # Construct bitmask
    andl    %ecx, %r8d                              # Apply mask

.Lpgx4b_aligned_mskd:
    shll    $0x1c, %r8d                             # Shift out bits corresponding to zeroed lanes
    lzcntl  %r8d, %r9d                              # Index of last matching character relative msb
    notq    %r9                                     # Multiply by -1 and subtract 1
    leaq    (%r9, %rdx), %r8                        # Offset of last matching char relative %rdi
    leaq    (%rdi, %r8), %r9                        # Address of last matching char
    cmovncq %r9, %rax                               # Store if there was a matching char

    testl   %r10d, %r10d                            # Check if null byte was encountered
    jnz     .Lepi

    jmp     .Lchk128b

.Lpgx2b:
    cmpl    $PGSIZE - 0x02, %ecx                    # Check if reading word would cross boundary
    ja      .Lpgx1b

    movzxb  (%rdi), %r8d                            # Load first byte
    cmpl  %esi, %r8d                                # Compare against char
    cmoveq  %rdi, %rax                              # Store address on match

    testl   %r8d, %r8d                              # Check for null
    jz      .Lepi

    leaq    0x01(%rdi), %r9                         # Address of second byte
    movzxb  (%r9), %r8d                             # Load second byte
    cmpl    %esi, %r8d                              # Compare agaist char
    cmoveq  %r9, %rax                               # Store address on match

    testl   %r8d, %r8d                              # Check for null
    jz      .Lepi

    leaq    0x02(%rdx), %rdx

.Lpgx1b:
    leaq    (%rdi, %rdx), %rcx                      # Compute position in current page
    andl    $PGSIZE - 1, %ecx
    testl   %ecx, %ecx                              # Check if aligned to boundary
    jz      .Lchk128b

    leaq    (%rdi, %rdx), %r9                       # Address of byte to load
    movzxb  (%r9), %r8d                             # Load byte
    cmpl    %esi, %r8d                              # Compare against char
    cmoveq  %r9, %rax                               # Store if there was a matching char

    leaq    0x01(%rdx), %rdx

    testl   %r8d, %r8d                              # Check for null
    jnz     .Lchk128b

.Lepi:
    vzeroupper
    ret
