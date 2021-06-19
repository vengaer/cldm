    section .text

    default rel

    global cldm_avx2_memcmp

    %define VECMASK 0xffffffff

    %macro bytecpt 1
        not     r8d                             ; Invert
        tzcnt   edx, r8d                        ; Vector index of offending byte
        lea     rdx, [rdx + rcx + %1]           ; Compute offset

        movzx   eax, byte [rdi + rdx]           ; Load byte from first source
        movzx   ecx, byte [rsi + rdx]           ; Load byte from second source
        sub     eax, ecx                        ; Return value
        vzeroupper
    %endmacro

; AVX2-accelerated memcmp
; Params:
;   rdi: Address of first memory area
;   rsi: Address of second memory area
;   rdx: Number of bytes to compare
; Return:
;   eax: = 0 if the memory areas are the same,
;        < 0 if area [rdi] is less than [rsi],
;        > 0 if area [rdi] is greater than [rsi]
cldm_avx2_memcmp:
    vpcmpeqb    ymm0, ymm0, ymm0                ; All ones

    cmp     rdx, 0x20
    jb      .clt32b
    je      .ceq32b

    mov     ecx, 0x0100                         ; Address offset

    tzcnt   r8, rdi                             ; Least significant set bit
    cmp     r8d, 0x5                            ; Check if aligned
    jnb     .c256b

    vmovdqu ymm1, [rdi]                         ; Load ymmword
    vpcmpeqb    ymm2, ymm1, [rsi]               ; Compare first ymmword

    vptest  ymm2, ymm0                          ; Set carry if nand yields zero
    jnc     .c32bdiff

    lea     rcx, [rdi + rcx + 0x1f]             ; Compute offset for end of first 256-byte block
    and     rcx, -0x20
    sub     rcx, rdi

.c256b:
    cmp     rdx, rcx                            ; Check room for 256 bytes
    jb      .clt256b

    vmovdqa ymm1, [rdi + rcx - 0x0100]          ; Load 8 ymmwords from each source and compare
    vpcmpeqb    ymm2, ymm1, [rsi + rcx - 0x0100]

    vmovdqa ymm1, [rdi + rcx - 0x00e0]
    vpcmpeqb    ymm3, ymm1, [rsi + rcx - 0x00e0]

    vmovdqa ymm1, [rdi + rcx - 0x00c0]
    vpcmpeqb    ymm4, ymm1, [rsi + rcx - 0x00c0]

    vmovdqa ymm1, [rdi + rcx - 0x00a0]
    vpcmpeqb    ymm5, ymm1, [rsi + rcx - 0x00a0]

    vmovdqa ymm1, [rdi + rcx - 0x0080]
    vpcmpeqb    ymm6, ymm1, [rsi + rcx - 0x0080]

    vmovdqa ymm1, [rdi + rcx - 0x0060]
    vpcmpeqb    ymm7, ymm1, [rsi + rcx - 0x0060]

    vmovdqa ymm1, [rdi + rcx - 0x0040]
    vpcmpeqb    ymm8, ymm1, [rsi + rcx - 0x0040]

    vmovdqa ymm1, [rdi + rcx - 0x0020]
    vpcmpeqb    ymm9, ymm1, [rsi + rcx - 0x0020]

    vpand   ymm10, ymm2, ymm3                   ; Reduce results
    vpand   ymm11, ymm4, ymm5
    vpand   ymm10, ymm10, ymm11
    vpand   ymm11, ymm6, ymm7
    vpand   ymm10, ymm10, ymm11
    vpand   ymm11, ymm8, ymm9
    vpand   ymm10, ymm10, ymm11

    vptest  ymm10, ymm0                         ; Set carry if nand yields zero
    jnc     .c256bdiff

    cmp     rdx, rcx                            ; Check for remaining bytes
    lea     rcx, [rcx + 0x0100]                 ; Advance offset
    ja      .c256b
    xor     eax, eax
    vzeroupper
    ret

.clt256b:
    sub     rcx, 0x80
    cmp     rdx, rcx                            ; Check room for 128 bytes
    jb      .clt128b

    vmovdqa ymm1, [rdi + rcx - 0x80]            ; Load 4 ymmwords from each source and compare
    vpcmpeqb    ymm2, ymm1, [rsi + rcx - 0x80]

    vmovdqa ymm1, [rdi + rcx - 0x60]
    vpcmpeqb    ymm3, ymm1, [rsi + rcx - 0x60]

    vmovdqa ymm1, [rdi + rcx - 0x40]
    vpcmpeqb    ymm4, ymm1, [rsi + rcx - 0x40]

    vmovdqa ymm1, [rdi + rcx - 0x20]
    vpcmpeqb    ymm5, ymm1, [rsi + rcx - 0x20]

    vpand   ymm6, ymm2, ymm3                    ; Reduce
    vpand   ymm7, ymm4, ymm5
    vpand   ymm6, ymm6, ymm7

    vptest  ymm6, ymm0                          ; Set carry if nand yields zero
    jnc     .clt256bdiff

    cmp     rdx, rcx                            ; Check if done
    jna     .epilogue

    add     rcx, 0x80

.clt128b:
    sub     rcx, 0x40
    cmp     rdx, rcx                            ; Check room for 64 bytes
    jb      .clt64b

    vmovdqa ymm1, [rdi + rcx - 0x40]            ; Load 2 ymmwords from each source and compare
    vpcmpeqb    ymm2, ymm1, [rsi + rcx - 0x40]

    vmovdqa ymm1, [rdi + rcx - 0x20]
    vpcmpeqb    ymm3, ymm1, [rsi + rcx - 0x20]

    vpand   ymm4, ymm2, ymm3                    ; Reduce

    vptest  ymm4, ymm0                          ; Set carry if nand yields zero
    jnc     .clt128bdiff

    cmp     rdx, rcx                            ; Check if done
    jna     .epilogue

    add     rcx, 0x40

.clt64b:
    sub     rcx, 0x20
    cmp     rdx, rcx                            ; Check room for 32 bytes
    jb      .clast32

    vmovdqa ymm1, [rdi + rcx - 0x20]            ; Load ymmword
    vpcmpeqb    ymm2, ymm1, [rsi + rcx - 0x20]

    vptest  ymm2, ymm0                          ; Set carry if nand yields zero
    jnc     .clt64bdiff

    cmp     rdx, rcx                            ; Check if done
    jna     .epilogue

    add     rcx, 0x20

.clast32:
    lea     r8, [rcx - 0x20]                    ; Subtract from offset to allow for loading unaligned ymmword
    sub     rcx, rdx
    sub     r8, rcx
    vmovdqu ymm1, [rdi + r8]                    ; Load
    vpcmpeqb    ymm2, ymm1, [rsi + r8]          ; Compare

    vptest  ymm2, ymm0
    jc      .epilogue

    vpmovmskb   ecx, ymm2                       ; Extract bitmask
    not     ecx                                 ; Invert
    tzcnt   edx, ecx                            ; Vector index of offending byte
    lea     rdx, [rdx + r8]                     ; Compute offset

    movzx   eax, byte [rdi + rdx]               ; Load byte from first source
    movzx   ecx, byte [rsi + rdx]               ; Load byte from second source
    sub     eax, ecx                            ; Return value
    vzeroupper
    ret

.c256bdiff:
    vpmovmskb   r8d, ymm2                       ; Extract bitmask
    cmp     r8d, VECMASK                        ; Check for offending byte
    jne     .c256bdiff001f

    vpmovmskb   r8d, ymm3
    cmp     r8d, VECMASK
    jne     .c256bdiff203f

    vpmovmskb   r8d, ymm4
    cmp     r8d, VECMASK
    jne     .c256bdiff405f

    vpmovmskb   r8d, ymm5
    cmp     r8d, VECMASK
    jne     .c256bdiff607f

    vpmovmskb   r8d, ymm6
    cmp     r8d, VECMASK
    jne     .c256bdiff809f

    vpmovmskb   r8d, ymm7
    cmp     r8d, VECMASK
    jne     .c256bdiffa0bf

    vpmovmskb   r8d, ymm8
    cmp     r8d, VECMASK
    jne     .c256bdiffc0df

    vpmovmskb   r8d, ymm9                       ; Offender not found in earlier ymmwords, must be here
    bytecpt -0x20
    ret

.c256bdiff001f:
    bytecpt -0x0100
    ret

.c256bdiff203f:
    bytecpt -0xe0
    ret

.c256bdiff405f:
    bytecpt -0xc0
    ret

.c256bdiff607f:
    bytecpt -0xa0
    ret

.c256bdiff809f:
    bytecpt -0x80
    ret

.c256bdiffa0bf:
    bytecpt -0x60
    ret

.c256bdiffc0df:
    bytecpt -0x40
    ret

.clt256bdiff:
    vpmovmskb   r8d, ymm2                       ; Extract bitmask
    cmp     r8d, VECMASK                        ; Check for offending byte
    jne     .clt256bdiff001f

    vpmovmskb   r8d, ymm3
    cmp     r8d, VECMASK
    jne     .clt256bdiff203f

    vpmovmskb   r8d, ymm4
    cmp     r8d, VECMASK
    jne     .clt256bdiff405f

    vpmovmskb   r8d, ymm5                       ; Offender not found in earlier ymmwords, must be here
    bytecpt -0x20
    ret

.clt256bdiff001f:
    bytecpt -0x80
    ret

.clt256bdiff203f:
    bytecpt -0x60
    ret

.clt256bdiff405f:
    bytecpt -0x40
    ret

.clt128bdiff:
    vpmovmskb   r8d, ymm2                       ; Extract bitmask
    cmp     r8d, VECMASK                        ; Check for offending byte
    jne     .clt128bdiff001f

    vpmovmskb   r8d, ymm3                       ; Offender not found in previous ymmword, must be here
    bytecpt -0x20
    ret

.clt128bdiff001f:
    bytecpt -0x40
    ret

.clt64bdiff:
    vpmovmskb   r8d, ymm2
    bytecpt -0x20
    ret

.epilogue:
    xor     eax, eax
    vzeroupper
    ret

.c32bdiff:
    vpmovmskb   r8d, ymm2                       ; Extract bitmask
    not     r8d                                 ; Invert
    tzcnt   edx, r8d                            ; Offset of offending byte
    lea     r8, [rcx + rdx - 0x0100]
    movzx   eax, byte [rdi + r8]                ; First byte at offending offset
    movzx   ecx, byte [rsi + r8]                ; Second byte at offending offset
    sub     eax, ecx                            ; Compute return value
    vzeroupper
    ret

.ceq32b:
    vpcmpeqb    ymm0, ymm0, ymm0                ; All ones
    vmovdqu ymm1, [rdi]                         ; First ymmword
    vpcmpeqb    ymm2, ymm1, [rsi]               ; Compare for equality

    vptest  ymm2, ymm0                          ; Set carry if nand yields zero
    jnc     .ceq32bdiff
    xor     eax, eax                            ; Memory areas match
    vzeroupper
    ret

.ceq32bdiff:
    vpmovmskb   ecx, ymm2                       ; Extract bitmask
    not     ecx                                 ; Invert bitmask
    tzcnt   edx, ecx                            ; Offset of offending byte
    movzx   eax, byte [rdi + rdx]               ; Read first byte at offending offset
    movzx   ecx, byte [rsi + rdx]               ; Read second byte at offending offset
    sub     eax, ecx                            ; Return value less than 0 first area is less
    vzeroupper
    ret

.clt32b:
    xor     r8d, r8d

    cmp     rdx, 0x10
    jb      .clt16b

    vmovdqu xmm1, [rdi]                         ; First xmmword
    vpcmpeqb    xmm2, xmm1, [rsi]               ; Compare to second xmmword

    add     r8d, 0x10
    vptest  xmm2, xmm0                          ; Set carry if not equal
    jc      .clt16b

    vpmovmskb   ecx, xmm2                       ; Extract bitmask
    not     ecx                                 ; Invert bitmask
    tzcnt   r8d, ecx                            ; Offset of offending byte
    movzx   eax, byte [rdi + r8]                ; First byte at offending offset
    movzx   ecx, byte [rsi + r8]                ; Second byte at offending offset
    sub     eax, ecx                            ; Return negative value if first area is less
    vzeroupper
    ret

.clt16b:
    xor     eax, eax
    cmp     r8d, edx                            ; Check if end has been reached
    je      .residual_done

.rdbyte:
    movzx   eax, byte [rdi + r8]                ; Read first byte
    movzx   ecx, byte [rsi + r8]                ; Read second byte

    sub     eax, ecx                            ; Compare bytes
    jnz     .residual_done

    add     r8d, 1
    cmp     r8d, edx
    jb      .rdbyte

.residual_done:
    vzeroupper
    ret
