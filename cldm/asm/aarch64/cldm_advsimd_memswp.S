    .section .text

.globl cldm_advsimd_memswp
// Swap x2 bytes between x0 and x1
// Params:
//   x0: Address of first memory area
//   x1: Address of second memory area
//   x2: Number of bytes to swap
// Return:
//   -
cldm_advsimd_memswp:
    tst     x2, x2
    b.eq    .Lepi

    add     x6, x0, x2                                              // Address of first byte after end of x0
    add     x7, x1, x2                                              // Address of first byte after end of x1

    add     x3, x0, #0x20                                           // Address of first aligned load from x0
    and     x3, x3, #-0x20

    add     x4, x1, #0x20                                           // Address of first aligned load from x1
    and     x4, x4, #-0x20

    sub     x6, x6, #0x20                                           // Address 32 bytes before end of x0
    cmp     x6, x3                                                  // Check if enough space for aligning
    b.le    .Lswp32b

    ld1     {v0.16b, v1.16b}, [x0]                                  // Load 32 bytes unaligned
    ld1     {v16.16b, v17.16b}, [x1]

    ld1     {v2.16b, v3.16b}, [x3]                                  // Load 32 bytes aligned with potential overlap
    ld1     {v18.16b, v19.16b}, [x4]

    st1     {v16.16b, v17.16b}, [x0]                                // Store first set unaligned
    st1     {v0.16b, v1.16b}, [x1]

    st1     {v18.16b, v19.16b}, [x3], #0x20                         // Store first aligned set with potential overlap
    st1     {v2.16b, v3.16b}, [x4], #0x20

    sub     x6, x6, #0x60                                           // Address 128 bytes before end of x0

.Lswp128b:
    cmp     x6, x3
    b.lo    .Lswp64b

    ld1     {v0.16b, v1.16b, v2.16b, v3.16b}, [x3]                  // Swap first 64 bytes
    ld1     {v16.16b, v17.16b, v18.16b, v19.16b}, [x4]
    st1     {v16.16b, v17.16b, v18.16b, v19.16b}, [x3], #0x40
    st1     {v0.16b, v1.16b, v2.16b, v3.16b}, [x4], #0x40

    ld1     {v4.16b, v5.16b, v6.16b, v7.16b}, [x3]                  // Swap last 64 bytes
    ld1     {v20.16b, v21.16b, v22.16b, v23.16b}, [x4]
    st1     {v20.16b, v21.16b, v22.16b, v23.16b}, [x3], #0x40
    st1     {v4.16b, v5.16b, v6.16b, v7.16b}, [x4], #0x40

    b.hi    .Lswp128b
    ret

.Lswp64b:
    add     x6, x6, #0x40                                           // Address 64 bytes before end of x0
    cmp     x6, x3
    b.lo    .Lswp32b

    ld1     {v0.16b, v1.16b, v2.16b, v3.16b}, [x3]                  // Swap 64 bytes
    ld1     {v16.16b, v17.16b, v18.16b, v19.16b}, [x4]
    st1     {v16.16b, v17.16b, v18.16b, v19.16b}, [x3], #0x40
    st1     {v0.16b, v1.16b, v2.16b, v3.16b}, [x4], #0x40

    b.eq    .Lepi

.Lswp32b:
    add     x6, x6, #0x20                                           // Address 32 bytes before end of x0
    cmp     x6, x3
    b.lo    .Lswp16b

    ld1     {v0.16b, v1.16b}, [x3]                                  // Load first set of 32 bytes
    ld1     {v16.16b, v17.16b}, [x4]

    ld1     {v2.16b, v3.16b}, [x6]                                  // Load second set

    sub     x7, x7, #0x20                                           // Address 32 bytes before end of x1
    ld1     {v18.16b, v19.16b}, [x7]

    st1     {v16.16b, v17.16b}, [x3]                                // Store first set
    st1     {v0.16b, v1.16b}, [x4]

    b.eq    .Lepi                                                   // No need to store again if sets overlap completely

    st1     {v18.16b, v19.16b}, [x6]                                // Store second set
    st1     {v2.16b, v3.16b}, [x7]
    ret

.Lswp16b:
    add     x6, x6, #0x10                                           // Address 16 bytes before end of x0
    cmp     x6, x3
    b.lo    .Lswp8b

    ld1     {v0.16b}, [x3]                                          // Load first set of 16 bytes
    ld1     {v16.16b}, [x4]

    ld1     {v1.16b}, [x6]                                          // Load second set
    sub     x7, x7, #0x10                                           // Address 16 bytes before end of x1
    ld1     {v17.16b}, [x7]

    st1     {v16.16b}, [x3]                                         // Store first set
    st1     {v0.16b}, [x4]

    b.eq    .Lepi                                                   // Complete overlap

    st1     {v17.16b}, [x6]
    st1     {v1.16b}, [x7]
    ret

.Lswp8b:
    add     x6, x6, #0x08
    cmp     x6, x3
    b.lo    .Lswp4b

    ldr     x9, [x3]                                                // First set
    ldr     x10, [x4]

    ldr     x11, [x6]                                               // Second set
    ldr     x12, [x7, #-0x08]

    str     x10, [x3]                                               // Store first set
    str     x9, [x4]

    b.eq    .Lepi

    str     x12, [x6]                                               // Store second set
    str     x11, [x7, #-0x08]
    ret

.Lswp4b:
    add     x6, x6, #0x04
    cmp     x6, x3
    b.lo    .Lswp2b

    ldr     w9, [x3]                                                // First set
    ldr     w10, [x4]

    ldr     w11, [x6]                                               // Second set
    ldr     w12, [x7, #-0x04]

    str     w10, [x3]                                               // Store first set
    str     w9, [x4]

    b.eq    .Lepi

    str     w12, [x6]                                               // Store second set
    str     w11, [x7, #-0x04]
    ret

.Lswp2b:
    add     x6, x6, #0x02
    cmp     x6, x3
    b.lo    .Lswp1b

    ldrh    w9, [x3]                                                // First set of half words
    ldrh    w10, [x4]

    ldrh    w11, [x6]                                               // Second set
    ldrh    w12, [x7, #-0x02]

    strh    w10, [x3]                                               // Store first set
    strh    w9, [x4]

    b.eq    .Lepi

    strh    w12, [x6]                                               // Store second set
    strh    w11, [x7, #-0x02]
    ret

.Lswp1b:
    ldrb    w9, [x3]
    ldrb    w10, [x4]
    strb    w10, [x3]
    strb    w9, [x4]
.Lepi:
    ret

