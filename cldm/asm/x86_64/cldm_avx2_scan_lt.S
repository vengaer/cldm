    .section .text

    #include "asm/system.h"

.macro  cmask ymmword
    vptest  \ymmword, \ymmword                      # Check if relevant byte was found
    cmovnzl %ecx, %edx                              # If found, set offset to 0
    leaq    (%rax, %rdx), %rax                      # Conditionally advance offset
.endm

.globl cldm_avx2_scan_lt
# Scan byte sequence and return index of
# first byte less than %esi. Similar to
# strlen but with every value less than %esi
# being treated like 0
# Params:
#   rdi: Address of byte sequence
#   esi: Upper bound for byte to search
#        for, non-inclusive
# Return:
#   rax: Offset of first byte less than %esi
cldm_avx2_scan_lt:
    xorl    %eax, %eax                              # Offset

    vpxor   %xmm2, %xmm2, %xmm2                     # Shuffle mask
    vmovd   %esi, %xmm1                             # Insert dword
    vpshufb %xmm2, %xmm1, %xmm0                     # Broadcast byte to all lanes
    vinserti128 $0x1, %xmm0, %ymm0, %ymm15          # Copy low xmmword to high

    movl    %edi, %ecx
    andl    $PGSIZE - 1, %ecx                       # Mask out page offset
    cmpl    $PGSIZE - 0x20, %ecx                    # Check whether reading ymmword would cross page boundary
    ja      .Lpgx16b

    vmovdqu (%rdi), %ymm0                           # Single unaligned read
    vpcmpgtb    %ymm0, %ymm15, %ymm4
    vptest  %ymm4, %ymm4
    jnz     .Lymmwd0

    leaq    0x20(%rdi), %rax                        # Align next read to 32 byte boundary
    andq    $-0x20, %rax
    subq    %rdi, %rax

    .align 16
.Lchk128b_prologue:
    leaq    (%rdi, %rax), %rcx
    andl    $PGSIZE - 1, %ecx
    cmpl    $PGSIZE - 0x80, %ecx                    # Check whether reading 4 ymmwords would cross page boundary
    ja      .Lpgx_aligned

.Lchk128b:
    vmovdqa 0x00(%rdi, %rax), %ymm0                 # Read 4 ymmwords and compare against limit
    vpcmpgtb    %ymm0, %ymm15, %ymm4
    vmovdqa 0x20(%rdi, %rax), %ymm1
    vpcmpgtb    %ymm1, %ymm15, %ymm5
    vmovdqa 0x40(%rdi, %rax), %ymm2
    vpcmpgtb    %ymm2, %ymm15, %ymm6
    vmovdqa 0x60(%rdi, %rax), %ymm3
    vpcmpgtb    %ymm3, %ymm15, %ymm7

    vpor    %ymm4, %ymm5, %ymm8                     # Reduce
    vpor    %ymm6, %ymm7, %ymm9
    vpor    %ymm8, %ymm9, %ymm10

    leaq    0x80(%rax), %rax

    vptest  %ymm10, %ymm10
    jz      .Lchk128b_prologue

    subq    $0x80, %rax                             # Start of last 128-byte block
    movl    $0x20, %edx                             # Size of ymmword
    xorl    %ecx, %ecx                              # All zeroes for conditionally disabling offset

    cmask   %ymm4                                   # Compute address of first ymmword with relevant byte
    cmask   %ymm5
    cmask   %ymm6

    vmovdqa (%rdi, %rax), %ymm0                     # Load first ymmword with relevant byte
    vpcmpgtb    %ymm0, %ymm15, %ymm4                # Compare against limit
    vpmovmskb   %ymm4, %ecx                         # Extract bitmask
    tzcntl  %ecx, %edx                              # Offset in ymmword
    leaq    (%rax, %rdx), %rax                      # Add to total offset
    vzeroupper
    ret

.Lymmwd0:
    vpmovmskb   %ymm4, %ecx
    tzcntl  %ecx, %eax
    vzeroupper
    ret

.Lpgx_aligned:
    vmovdqa (%rdi, %rax), %ymm0                     # Process single aligned ymmword
    vpcmpgtb    %ymm0, %ymm15, %ymm4
    vptest  %ymm4, %ymm4
    jnz     .Lpgx_aligned_epi

    leaq    0x20(%rax), %rax                        # Advance offset
    leaq    (%rdi, %rax), %rcx                      # Address of next load
    andl    $PGSIZE - 1, %ecx                       # Compute offset in page
    testl   %ecx, %ecx                              # Check if aligned to page boundary
    jz      .Lchk128b
    jmp     .Lpgx_aligned

.Lpgx_aligned_epi:
    vpmovmskb   %ymm4, %ecx                         # Extract bitmask
    tzcntl  %ecx, %edx                              # Offset in ymmword
    leaq    (%rax, %rdx), %rax                      # Return value
    vzeroupper
    ret

.Lpgx16b:
    cmpl    $PGSIZE - 0x10, %ecx                    # Check if reading single xmmword would cross boundary
    ja      .Lpgx8b

    vmovdqu (%rdi), %xmm0                           # Check single xmmword
    vpcmpgtb    %xmm0, %xmm15, %xmm4

    leaq    0x10(%rax), %rax                        # Advance offset
    addl    $0x10, %ecx                             # Advanve in-page offset

    vptest  %xmm4, %xmm4                            # Check for relevant byte
    jz      .Lpgx8b

    vpmovmskb   %xmm4, %esi                         # Extract bitmask
    tzcntl  %esi, %edx                              # Offset in xmmword
    leaq    -0x10(%rax, %rdx), %rax                 # Return value
    vzeroupper
    ret

.Lpgx8b:
    cmpl    $PGSIZE, %ecx                           # Check if aligned to page boundary
    je      .Lchk128b
    cmpl    $PGSIZE - 0x08, %ecx                    # Check if reading single qword would cross boundary
    ja      .Lpgx4b

    vmovq   (%rdi, %rax), %xmm0                     # Check qword
    vpcmpgtb    %xmm0, %xmm15, %xmm4
    vpmovmskb   %xmm4, %edx                         # Extract bitmask
    leaq    0x08(%rax), %rax                        # Advance offsets
    addl    $0x08, %ecx
    testl   $0xff, %edx                             # Check for relevant byte
    jz      .Lpgx4b

    tzcntl  %edx, %esi                              # Offset in qword
    leaq    -0x08(%rax, %rsi), %rax                 # Return value
    vzeroupper
    ret

.Lpgx4b:
    cmpl    $PGSIZE, %ecx                           # Check if aligned to boundary
    je      .Lchk128b
    cmpl    $PGSIZE - 0x04, %ecx                    # Check if reading single dword would cross boundary
    ja      .Lpgx2b

    vmovd   (%rdi, %rax), %xmm0                     # Check dword
    vpcmpgtb    %xmm0, %xmm15, %xmm4
    vpmovmskb   %xmm4, %edx                         # Extract bitmask
    leaq    0x04(%rax), %rax                        # Advance offsets
    addl    $0x04, %ecx
    testl   $0x0f, %edx                             # Check for relevant byte
    jz      .Lpgx2b

    tzcntl  %edx, %esi                              # Offset in dword
    leaq    -0x04(%rax, %rsi), %rax                 # Return value
    vzeroupper
    ret

.Lpgx2b:
    cmpl    $PGSIZE, %ecx                           # Check if aligned to boundary
    je      .Lchk128b
    cmpl    $PGSIZE - 0x02, %ecx                    # Check if reading single word would cross boundary
    ja      .Lpgx1b

    movzxw  (%rdi, %rax), %edx                      # Read word

    movl    %edx, %r8d
    andl    $0x0f, %edx                             # First byte

    shrl    $0x08, %r8d                             # Second byte

    movl    $0x01, %r9d                             # Step size
    xorl    %r10d, %r10d                            # For conditionally zeroing step size
    xorl    %r11d, %r11d

    cmpl    %edx, %esi                              # Check first byte
    cmovbl  %r10d, %r9d                             # Zero if condition fulfilled
    setbb   %r11b                                   # Set to indicate relevant byte found
    addq    %r9, %rax                               # Advance offset

    cmpl    %r8d, %esi                              # Check second byte
    cmovbl  %r10d, %r9d                             # Zero if condition fulfilled
    setbb   %r11b                                   # Mark found
    addq    %r9, %rax                               # Advance offset

    leaq    0x02(%rax), %rax                        # Advance offsets
    addl    $0x02, %ecx

    test    %r11d, %r11d                            # Check if relevant byte was encountered
    jz      .Lpgx1b

    leaq    -0x02(%rax), %rax                       # Return value
    vzeroupper
    ret

.Lpgx1b:
    cmpl    $PGSIZE, %ecx                           # Check if aligned to boundary
    jz      .Lchk128b

    leaq    0x01(%rax), %rax                        # Advance offset
    cmpb    (%rdi, %rax), %sil                      # Check single byte
    jnb     .Lchk128b

    subq    $0x01, %rax
    vzeroupper
    ret

