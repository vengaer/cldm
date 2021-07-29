    .section .text

.global cldm_neon_memswp
// Swap x2 bytes between x0 and x2
// Params:
//   x0: Address of first memory area
//   x1: Address of second memory area
//   x2: Number of bytes to swap
// Return:
//   -
cldm_neon_memswp:
    add     x3, x0, #0x20                                           // Offset of first aligned load
    and     x3, x3, #-0x20
    sub     x3, x3, x0

    add     x4, x3, #0x20                                           // Check if enough space for aligning
    cmp     x2, x4
    b.le    .Lswp16b_unaligned

    add     x5, x0, x3                                              // Addresses of first aligned loads
    add     x6, x1, x3

    ld1     {v0.16b, v1.16b}, [x0]                                  // Load first 32 bytes unaligned
    ld1     {v2.16b, v3.16b}, [x1]

    ld1     {v4.16b, v5.16b}, [x5]                                  // First aligned load
    ld1     {v6.16b, v7.16b}, [x6]

    st1     {v2.16b, v3.16b}, [x0]                                  // Unaligned write
    st1     {v0.16b, v1.16b}, [x1]

    st1     {v6.16b, v7.16b}, [x6]                                  // First aligned write
    st1     {v4.16b, v5.16b}, [x5]

    add     x4, x4, #0xa0                                           // Offset of end of next block to be swapped

    .align 16
.Lswp128b:
    cmp     x2, x4
    b.lt    .Lswp64b

    ld1     {v0.16b, v1.16b, v2.16b, v3.16b}, [x5]                  // Load 64 bytes from each source
    ld1     {v16.16b, v17.16b, v18.16b, v19.16b}, [x6]

    st1     {v16.16b, v17.16b, v18.16b, v19.16b}, [x5], #0x40       // Write and increment
    st1     {v0.16b, v1.16b, v2.16b, v3.16b}, [x6], #0x40

    ld1     {v4.16b, v5.16b, v6.16b, v7.16b}, [x5]                  // Repeat for another 64 bytes
    ld1     {v20.16b, v21.16b, v22.16b, v23.16b}, [x6]

    st1     {v20.16b, v21.16b, v22.16b, v23.16b}, [x5], #0x40
    st1     {v4.16b, v5.16b, v6.16b, v7.16b}, [x6], #0x40

    add     x4, x4, 0x80
    b.gt        .Lswp128b
    ret

.Lswp64b:
    sub     x4, x4, #0x40
    cmp     x2, x4                                                  // Check against upper bound
    b.lt    .Lswp32b

    ld1     {v0.16b, v1.16b, v2.16b, v3.16b}, [x5]                  // Swap 64 bytes
    ld1     {v16.16b, v17.16b, v18.16b, v19.16b}, [x6]
    st1     {v16.16b, v17.16b, v18.16b, v19.16b}, [x5], #0x40
    st1     {v0.16b, v1.16b, v2.16b, v3.16b}, [x6], #0x40

    b.eq    .Lepi

    add     x4, x4, #0x40

.Lswp32b:
    sub     x4, x4, #0x20
    cmp     x2, x4                                                  // Check against upper bound
    b.lt    .Lswp16b

    sub     x7, x4, #0x20

    add     x8, x0, x7                                              // Addresses of last loads
    add     x9, x1, x8

    ld1     {v0.16b, v1.16b}, [x5]                                  // Load first set of 32 bytes
    ld1     {v16.16b, v17.16b}, [x6]

    ld1     {v2.16b, v3.16b}, [x8]                                  // Load final set of 32 bytes
    ld1     {v18.16b, v19.16b}, [x9]

    st1     {v16.16b, v17.16b}, [x5]                                // Write first set
    st1     {v0.16b, v1.16b}, [x6]

    b.eq    .Lepi

    st1     {v18.16b, v19.16b}, [x8]                                // Write final set
    st1     {v2.16b, v3.16b}, [x9]
    ret

.Lswp16b:
    sub     x4, x4, #0x10
    cmp     x2, x4                                                  // Check against upper bound
    b.lt    .Lswp8b

    sub     x7, x5, #0x10

    add     x8, x0, x7                                              // Addresses of last loads
    add     x9, x1, x7

    ld1     {v0.16b}, [x5]                                          // First set of 16 bytes
    ld1     {v16.16b}, [x6]

    ld1     {v1.16b}, [x8]                                          // Final set of 16 bytes
    ld1     {v17.16b}, [x9]

    st1     {v16.16b}, [x5]                                         // Write first set
    st1     {v0.16b}, [x6]

    b.eq    .Lepi

    st1     {v17.16b}, [x8]                                         // Write final set
    st1     {v1.16b}, [x9]
    ret

.Lswp8b:
    sub     x4, x4, #0x08
    cmp     x2, x4                                                  // Check against upper bound
    b.lt    .Lswp4b

    sub     x7, x5, #0x08

    add     x8, x0, x7                                              // Addresses of last loads
    add     x9, x1, x7

    ld1     {v0.8b}, [x5]                                           // First set of 8 bytes
    ld1     {v16.8b}, [x6]

    ld1     {v1.8b}, [x8]                                           // Second set of 8 bytes
    ld1     {v17.8b}, [x9]

    st1     {v16.8b}, [x5]                                          // Write first set
    st1     {v0.8b}, [x6]

    b.eq    .Lepi

    st1     {v17.8b}, [x8]                                          // Write second set
    st1     {v1.8b}, [x9]
    ret

.Lswp4b:
    sub     x4, x4, #0x04
    cmp     x2, x4
    b.lt    .Lswp2b

    sub     x7, x5, #0x04
    add     x8, x0, x7                                              // Addresses of last loads
    add     x9, x1, x7

    ldr     w10, [x5]                                               // Load first set of words
    ldr     w11, [x6]

    ldr     w12, [x8]                                               // Final set of words
    ldr     w13, [x9]

    str     w11, [x5]                                               // Write first set of words
    str     w10, [x6]

    b.eq    .Lepi

    str     w13, [x8]                                               // Store second set
    str     w12, [x9]
    ret

.Lswp2b:
    sub     x4, x4, #0x02
    cmp     x2, x4
    b.lt    .Lswp1b

    sub     x7, x4, #0x02
    add     x8, x0, x7                                              // Addresses of last loads
    add     x9, x1, x7

    ldrh    w10, [x5]                                               // Load first set of half words
    ldrh    w11, [x6]

    ldrh    w12, [x8]                                               // Second set
    ldrh    w13, [x9]

    strh    w11, [x5]                                               // Write first set
    strh    w10, [x6]

    b.eq    .Lepi

    strh    w13, [x8]                                               // Write second set
    strh    w12, [x9]
    ret

.Lswp1b:
    sub     x7, x4, #0x01
    add     x8, x0, x7                                              // Addresses of last loads
    add     x9, x1, x7

    ldrb    w10, [x5]                                               // Load first set of bytes words
    ldrb    w11, [x6]

    ldrb    w12, [x8]                                               // Second set
    ldrb    w13, [x9]

    strb    w11, [x5]                                               // Write first set
    strb    w10, [x6]

    b.eq    .Lepi

    strb    w13, [x8]                                               // Write second set
    strb    w12, [x9]
    ret

.Lepi:
    ret

.Lswp16b_unaligned:
