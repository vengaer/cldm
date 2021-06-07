    section .text
    default rel

    global cldm_avx2_memswp

; Swap rdx bytes between [rdi] and
; [rsi]
; Params:
;     rdi: Address of first memory area
;     rsi: Adderss of second memory area
;     rdx: Number of bytes to swap
; Return:
;     -
cldm_avx2_memswp:
    cmp     rdx, 0x20
    jb      .uswpxmmwd
    je      .uswpymmwd

    section .data
.align_table:
    dq .aswpbyte
    dq .aswpword
    dq .aswpdword
    dq .aswpqword
    dq .aswpxmmword
    dq .aswpymmwords

    section .text

    xor     eax, eax
    mov     r10d, 0x05
    lea     rcx, [.align_table]
    tzcnt   r8, rdi
    cmp     r8d, 0x04
    cmova   r8d, r10d
    shl     r8d, 0x03
    jmp     [rcx + r8]

.aswpbyte:
    movzx   r8d, byte [rdi]
    movzx   r9d, byte [rsi]
    mov     byte [rdi], r9b
    mov     byte [rsi], r8b

    lea     eax, [eax + 0x01]

    lea     r9, [rdi + rax]
    tzcnt   r8, r9
    cmp     r8d, 0x04
    cmova   r8d, r10d
    shl     r8d, 0x03
    jmp     [rcx + r8]

.aswpword:
    movzx   r8d, word [rdi + rax]
    movzx   r9d, word [rsi + rax]
    mov     word [rdi + rax], r9w

    mov     word [rsi + rax], r8w

    lea     eax, [eax + 0x02]

    lea     r9, [rdi + rax]
    tzcnt   r8, r9
    cmp     r8d, 0x04
    cmova   r8d, r10d
    shl     r8d, 0x03
    jmp     [rcx + r8]
.aswpdword:
    mov     r8d, dword [rdi + rax]
    mov     r9d, dword [rsi + rax]
    mov     dword [rdi + rax], r9d
    mov     dword [rsi + rax], r8d

    lea     eax, [eax + 0x04]

    lea     r9, [rdi + rax]
    tzcnt   r8, r9
    cmp     r8d, 0x04
    cmova   r8d, r10d
    shl     r8d, 0x03
    jmp     [rcx + r8]
.aswpqword:
    mov     r8, qword [rdi + rax]
    mov     r9, qword [rsi + rax]
    mov     qword [rdi + rax], r9
    mov     qword [rsi + rax], r8

    lea     eax, [eax + 0x08]

    lea     r9, [rdi + rax]
    tzcnt   r8, r9
    cmp     r8d, 0x04
    cmova   r8d, r10d
    shl     r8d, 0x03
    jmp     [rcx + r8]
.aswpxmmword:
    vmovdqa xmm0, [rdi + rax]
    vmovdqu xmm1, [rsi + rax]
    vmovdqa [rdi + rax], xmm1
    vmovdqu [rsi + rax], xmm0

    lea     eax, [eax + 0x10]

.aswpymmwords:
    add     rax, 0x80                       ; Address offset
.aswp128b:
    cmp     rdx, rax
    jb      .aswp64b

    vmovdqa ymm0, [rdi + rax - 0x80]
    vmovdqu ymm1, [rsi + rax - 0x80]
    vmovdqa [rdi + rax - 0x80], ymm1
    vmovdqu [rsi + rax - 0x80], ymm0

    vmovdqa ymm0, [rdi + rax - 0x60]
    vmovdqu ymm1, [rsi + rax - 0x60]
    vmovdqa [rdi + rax - 0x60], ymm1
    vmovdqu [rsi + rax - 0x60], ymm0

    vmovdqa ymm0, [rdi + rax - 0x40]
    vmovdqu ymm1, [rsi + rax - 0x40]
    vmovdqa [rdi + rax - 0x40], ymm1
    vmovdqu [rsi + rax - 0x40], ymm0

    vmovdqa ymm0, [rdi + rax - 0x20]
    vmovdqu ymm1, [rsi + rax - 0x20]
    vmovdqa [rdi + rax - 0x20], ymm1
    vmovdqu [rsi + rax - 0x20], ymm0

    lea     rax, [rax + 0x80]
    ja      .aswp128b
    vzeroupper
    ret

.aswp64b:
    sub     rax, 0x40
    cmp     rdx, rax
    jb      .aswp32b

    vmovdqa ymm0, [rdi + rax - 0x40]
    vmovdqu ymm1, [rsi + rax - 0x40]
    vmovdqa [rdi + rax - 0x40], ymm1
    vmovdqu [rsi + rax - 0x40], ymm0

    vmovdqa ymm0, [rdi + rax - 0x20]
    vmovdqu ymm1, [rsi + rax - 0x20]
    vmovdqa [rdi + rax - 0x20], ymm1
    vmovdqu [rsi + rax - 0x20], ymm0

    je      .epilogue
    lea     rax, [rax + 0x40]

.aswp32b:
    sub     rax, 0x20
    cmp     rdx, rax
    jb      .aswp16b

    vmovdqa ymm0, [rdi + rax - 0x20]
    vmovdqu ymm1, [rsi + rax - 0x20]
    vmovdqa [rdi + rax - 0x20], ymm1
    vmovdqu [rsi + rax - 0x20], ymm0

    je      .epilogue
    lea     rax, [rax + 0x20]

.aswp16b:
    sub     rax, 0x10
    cmp     rdx, rax
    jb      .aswp8b

    vmovdqa xmm0, [rdi + rax - 0x10]
    vmovdqu xmm1, [rsi + rax - 0x10]
    vmovdqa [rdi + rax - 0x10], xmm1
    vmovdqu [rsi + rax - 0x10], xmm0

    je      .epilogue
    lea     rax, [rax + 0x10]

.aswp8b:
    sub     rax, 0x08
    cmp     rdx, rax
    jb      .aswp4b

    vmovq   xmm0, [rdi + rax - 0x08]
    vmovq   xmm1, [rsi + rax - 0x08]
    vmovq   [rdi + rax - 0x08], xmm1
    vmovq   [rsi + rax - 0x08], xmm0

    je      .epilogue
    lea     rax, [rax + 0x08]

.aswp4b:
    sub     rax, 0x04
    cmp     rdx, rax
    jb      .aswp2b

    vmovd   xmm0, [rdi + rax - 0x04]
    vmovd   xmm1, [rsi + rax - 0x04]
    vmovd   [rdi + rax - 0x04], xmm1
    vmovd   [rsi + rax - 0x04], xmm0

    je      .epilogue
    lea     rax, [rax + 0x04]

.aswp2b:
    sub     rax, 0x02
    cmp     rdx, rax
    jb      .aswp1b

    movzx   r8d, byte [rdi + rax - 0x02]
    movzx   r9d, byte [rsi + rax - 0x02]
    mov     byte [rdi + rax - 0x02], r9b
    mov     byte [rsi + rax - 0x02], r8b

    movzx   r8d, byte [rdi + rax - 0x01]
    movzx   r9d, byte [rsi + rax - 0x01]
    mov     byte [rdi + rax - 0x01], r9b
    mov     byte [rsi + rax - 0x01], r8b

    je      .epilogue
    lea     rax, [rax + 0x02]

.aswp1b:
    sub     rax, 0x01
    cmp     rdx, rax
    jb      .aswp1b

    movzx   r8d, byte [rdi + rax - 0x01]
    movzx   r9d, byte [rsi + rax - 0x01]
    mov     byte [rdi + rax - 0x01], r9b
    mov     byte [rsi + rax - 0x01], r8b

.epilogue:
    vzeroupper
    ret

.uswpymmwd:
    vmovdqu ymm0, [rdi]
    vmovdqu ymm1, [rsi]
    vmovdqu [rdi], ymm1
    vmovdqu [rsi], ymm0
    vzeroupper
    ret

.uswpxmmwd:
    mov     eax, 0x10

    cmp     edx, eax
    jb      .uswpqword

    vmovdqu xmm0, [rdi]
    vmovdqu xmm1, [rsi]
    vmovdqu [rdi], xmm1
    vmovdqu [rsi], ymm0

    add     eax, 0x10

.uswpqword:
    sub     eax, 0x08
    cmp     edx, eax
    jb      .uswpdword

    vmovq   xmm0, [rdi + rax - 0x08]
    vmovq   xmm1, [rsi + rax - 0x08]
    vmovq   [rdi + rax - 0x08], xmm1
    vmovq   [rsi + rax - 0x08], xmm0

    add     eax, 0x08

.uswpdword:
    sub     eax, 0x04
    cmp     edx, eax
    jb      .uswpwd

    vmovd   xmm0, [rdi + rax - 0x04]
    vmovd   xmm1, [rsi + rax - 0x04]
    vmovd   [rdi + rax - 0x04], xmm1
    vmovd   [rsi + rax - 0x04], xmm0

    add     eax, 0x04

.uswpwd:
    sub     eax, 0x02
    cmp     edx, eax
    jb      .uswpbyte

    movzx   r8d, byte [rdi + rax - 0x02]
    movzx   r9d, byte [rsi + rax - 0x02]
    mov     byte [rdi + rax - 0x02], r9b
    mov     byte [rsi + rax - 0x02], r8b


    movzx   r8d, byte [rdi + rax - 0x01]
    movzx   r9d, byte [rsi + rax - 0x01]
    mov     byte [rdi + rax - 0x01], r9b
    mov     byte [rsi + rax - 0x01], r8b

    add     eax, 0x02

.uswpbyte:
    sub     eax, 0x01
    cmp     edx, eax
    jb      .uswpdone

    movzx   r8d, byte [rdi + rax - 0x01]
    movzx   r9d, byte [rsi + rax - 0x01]
    mov     byte [rdi + rax - 0x01], r9b
    mov     byte [rsi + rax - 0x01], r8b

.uswpdone:
    vzeroupper
    ret
