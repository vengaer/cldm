    section .text

    default rel

    global cldm_avx2_memcpy

; AVX2-accelerated memcpy
; Params:
;   rdi: Destination address
;   rsi: Source address
;   rdx: Number of bytes to copy
; Return:
;   rax: Destination address
cldm_avx2_memcpy:
    mov     rax, rdi                        ; Return destination

    cmp     rdx, 0x20                       ; Check room for 32 bytes
    jb      .cpylt32b
    je      .cpyeq32b

    mov     ecx, 0x0100                     ; Address offset

    tzcnt   r8, rdi                         ; Least significant set bit
    cmp     r8d, 0x5                        ; Check if aligned
    jnb     .cpy256b

    vmovdqu ymm0, [rsi]                     ; Load ymmword
    vmovdqu [rdi], ymm0                     ; Write

    lea     rcx, [rdi + rcx + 0x1f]         ; End of offset for first 256-byte block
    and     rcx, -0x20                      ; Align
    sub     rcx, rdi

.cpy256b:
    cmp     rdx, rcx                        ; Check room for 256 bytes
    jb      .cpy128b

    vmovdqu ymm0, [rsi + rcx - 0x0100]      ; First ymmword
    vmovdqa [rdi + rcx - 0x0100], ymm0

    vmovdqu ymm0, [rsi + rcx - 0x00e0]      ; Second ymmword
    vmovdqa [rdi + rcx - 0x00e0], ymm0

    vmovdqu ymm0, [rsi + rcx - 0x00c0]      ; Third ymmword
    vmovdqa [rdi + rcx - 0x00c0], ymm0

    vmovdqu ymm0, [rsi + rcx - 0x00a0]      ; Fourth ymmword
    vmovdqa [rdi + rcx - 0x00a0], ymm0

    vmovdqu ymm0, [rsi + rcx - 0x0080]      ; Fifth ymmword
    vmovdqa [rdi + rcx - 0x0080], ymm0

    vmovdqu ymm0, [rsi + rcx - 0x0060]      ; Sixth ymmword
    vmovdqa [rdi + rcx - 0x0060], ymm0

    vmovdqu ymm0, [rsi + rcx - 0x0040]      ; Seventh ymmword
    vmovdqa [rdi + rcx - 0x0040], ymm0

    vmovdqu ymm0, [rsi + rcx - 0x0020]      ; Eighth ymmword
    vmovdqa [rdi + rcx - 0x0020], ymm0

    lea     rcx, [rcx + 0x0100]             ; Advance offset
    ja      .cpy256b                        ; Repeat if more bytes remain
    vzeroupper
    ret

.cpy128b:
    sub     rcx, 0x80
    cmp     rdx, rcx                        ; Check room for 128 bytes
    jb      .cpy64b

    vmovdqu ymm0, [rsi + rcx - 0x80]        ; First ymmword
    vmovdqa [rdi + rcx - 0x80], ymm0

    vmovdqu ymm0, [rsi + rcx - 0x60]        ; Second ymmword
    vmovdqa [rdi + rcx - 0x60], ymm0

    vmovdqu ymm0, [rsi + rcx - 0x40]        ; Third ymmword
    vmovdqa [rdi + rcx - 0x40], ymm0

    vmovdqu ymm0, [rsi + rcx - 0x20]        ; Fourth ymmword
    vmovdqa [rdi + rcx - 0x20], ymm0

    jna     .epilogue
    add     rcx, 0x80                       ; Advance offset

.cpy64b:
    sub     rcx, 0x40
    cmp     rdx, rcx                        ; Check room for 64 bytes
    jb      .cpy32b

    vmovdqu ymm0, [rsi + rcx - 0x40]        ; First ymmword
    vmovdqa [rdi + rcx - 0x40], ymm0

    vmovdqu ymm0, [rsi + rcx - 0x20]        ; Second ymmword
    vmovdqa [rdi + rcx - 0x20], ymm0

    jna     .epilogue
    add     rcx, 0x40

.cpy32b:
    sub     rcx, 0x20
    cmp     rdx, rcx                       ; Check room for 32 bytes
    jb      .cpylast32

    vmovdqu ymm0, [rsi + rcx - 0x20]        ; Single ymmword
    vmovdqa [rdi + rcx - 0x20], ymm0

    jna     .epilogue
    add     rcx, 0x20

.cpylast32:
    lea     r8, [rcx - 0x20]                ; Subtract from offset to allow for reading/writing
    sub     rcx, rdx                        ; unalinged ymmword
    sub     r8, rcx

    vmovdqu ymm0, [rsi + r8]                ; Final 32 bytes
    vmovdqu [rdi + r8], ymm0

.epilogue:
    vzeroupper
    ret

.cpyeq32b:
    vmovdqu ymm0, [rsi]                     ; Read ymmword
    vmovdqu [rdi], ymm0                     ; Write
    vzeroupper
    ret
.cpylt32b:
    xor     ecx, ecx                        ; Offset

    cmp     rdx, 0x10                       ; Check root for 16 bytes
    jb      .cpylt16b

    vmovdqu xmm0, [rsi]                     ; Read xmmword
    vmovdqu [rdi], xmm0                     ; Write
    add     ecx, 0x10
    cmp     ecx, edx                        ; Check if done
    jnb     .residual_done

.cpylt16b:
    movzx   r8d, byte [rsi + rcx]           ; Read byte
    mov     byte [rdi + rcx], r8b           ; Write

    add     ecx, 1
    cmp     ecx, edx
    jb      .cpylt16b

.residual_done:
    vzeroupper
    ret
