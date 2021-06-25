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
    cmp     rdx, 0x20                           ; Compare size against ymmword
    jb      .uswpxmmwd
    je      .uswpymmwd

    section .data
    align   8
.aligntbl:
    dq .aswpbyte
    dq .aswpword
    dq .aswpdword
    dq .aswpqword
    dq .aswpxmmword
    dq .aswpymmwords

    section .text

    xor     eax, eax                            ; Offset
    mov     r10d, 0x05                          ; For clamping jump offset
    lea     rcx, [.aligntbl]                    ; Load jump table
    tzcnt   r8, rdi                             ; Trailing zerobits
    cmp     r8d, 0x04
    cmova   r8d, r10d                           ; Clamp
    jmp     [rcx + r8 * 8]                      ; Jump to branch

.aswpbyte:
    movzx   r8d, byte [rdi]                     ; Load bytes
    movzx   r9d, byte [rsi]
    mov     byte [rdi], r9b                     ; Swap and store
    mov     byte [rsi], r8b

    lea     eax, [eax + 0x01]                   ; Advance offset

    lea     r9, [rdi + rax]                     ; Address of next byte in first source
    tzcnt   r8, r9                              ; Trailing zerobits
    cmp     r8d, 0x04
    cmova   r8d, r10d                           ; Clamp
    jmp     [rcx + r8 * 8]

.aswpword:
    movzx   r8d, word [rdi + rax]               ; Load words
    movzx   r9d, word [rsi + rax]
    mov     word [rdi + rax], r9w               ; Swap and store
    mov     word [rsi + rax], r8w

    lea     eax, [eax + 0x02]                   ; Advance offset

    lea     r9, [rdi + rax]                     ; Address of next byte in first source
    tzcnt   r8, r9
    cmp     r8d, 0x04
    cmova   r8d, r10d                           ; Clamp
    jmp     [rcx + r8 * 8]
.aswpdword:
    mov     r8d, dword [rdi + rax]              ; Load dwords
    mov     r9d, dword [rsi + rax]
    mov     dword [rdi + rax], r9d              ; Swap and store
    mov     dword [rsi + rax], r8d

    lea     eax, [eax + 0x04]                   ; Advance offset

    lea     r9, [rdi + rax]                     ; Address of next byte
    tzcnt   r8, r9
    cmp     r8d, 0x04
    cmova   r8d, r10d                           ; Clamp
    jmp     [rcx + r8 * 8]
.aswpqword:
    mov     r8, qword [rdi + rax]               ; Load qwords
    mov     r9, qword [rsi + rax]
    mov     qword [rdi + rax], r9               ; Swap and store
    mov     qword [rsi + rax], r8

    lea     eax, [eax + 0x08]                   ; Advance offset

    lea     r9, [rdi + rax]                     ; Address of next byte
    tzcnt   r8, r9
    cmp     r8d, 0x04
    cmova   r8d, r10d                           ; Clamp
    jmp     [rcx + r8 * 8]
.aswpxmmword:
    vmovdqa xmm0, [rdi + rax]                   ; Load xmmwords
    vmovdqu xmm1, [rsi + rax]
    vmovdqa [rdi + rax], xmm1                   ; Swap and store
    vmovdqu [rsi + rax], xmm0

    lea     eax, [eax + 0x10]                   ; Advance offset

.aswpymmwords:
    add     rax, 0x80                           ; Address offset after writing 4 ymmwords
.aswp128b:
    cmp     rdx, rax                            ; Check against upper bound
    jb      .aswp64b

    vmovdqa ymm0, [rdi + rax - 0x80]            ; Load, swap and store 4 ymmwords
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

    lea     rax, [rax + 0x80]                   ; Advance offset
    ja      .aswp128b                           ; Check for remaining bytes
    vzeroupper
    ret

.aswp64b:
    sub     rax, 0x40
    cmp     rdx, rax                            ; Check against upper bound
    jb      .aswp32b

    vmovdqa ymm0, [rdi + rax - 0x40]            ; Load, swap and store 2 ymmwords
    vmovdqu ymm1, [rsi + rax - 0x40]
    vmovdqa [rdi + rax - 0x40], ymm1
    vmovdqu [rsi + rax - 0x40], ymm0

    vmovdqa ymm0, [rdi + rax - 0x20]
    vmovdqu ymm1, [rsi + rax - 0x20]
    vmovdqa [rdi + rax - 0x20], ymm1
    vmovdqu [rsi + rax - 0x20], ymm0

    je      .epilogue
    lea     rax, [rax + 0x40]                   ; Advance offset

.aswp32b:
    sub     rax, 0x20
    cmp     rdx, rax                            ; Check against upper bound
    jb      .aswp16b

    vmovdqa ymm0, [rdi + rax - 0x20]            ; Load, swap and store ymmwords
    vmovdqu ymm1, [rsi + rax - 0x20]
    vmovdqa [rdi + rax - 0x20], ymm1
    vmovdqu [rsi + rax - 0x20], ymm0

    je      .epilogue
    lea     rax, [rax + 0x20]                   ; Advance offset

.aswp16b:
    sub     rax, 0x10
    cmp     rdx, rax                            ; Check against upper bound
    jb      .aswp8b

    vmovdqa xmm0, [rdi + rax - 0x10]            ; Load, swap and store xmmword
    vmovdqu xmm1, [rsi + rax - 0x10]
    vmovdqa [rdi + rax - 0x10], xmm1
    vmovdqu [rsi + rax - 0x10], xmm0

    je      .epilogue
    lea     rax, [rax + 0x10]                   ; Advance offset

.aswp8b:
    sub     rax, 0x08
    cmp     rdx, rax                            ; Check against upper bound
    jb      .aswp4b

    vmovq   xmm0, [rdi + rax - 0x08]            ; Load, swap and store qwords
    vmovq   xmm1, [rsi + rax - 0x08]
    vmovq   [rdi + rax - 0x08], xmm1
    vmovq   [rsi + rax - 0x08], xmm0

    je      .epilogue
    lea     rax, [rax + 0x08]                   ; Advance offset

.aswp4b:
    sub     rax, 0x04
    cmp     rdx, rax                            ; Check against upper bound
    jb      .aswp2b

    vmovd   xmm0, [rdi + rax - 0x04]            ; Load, swap and store dwords
    vmovd   xmm1, [rsi + rax - 0x04]
    vmovd   [rdi + rax - 0x04], xmm1
    vmovd   [rsi + rax - 0x04], xmm0

    je      .epilogue
    lea     rax, [rax + 0x04]                   ; Advance offset

.aswp2b:
    sub     rax, 0x02
    cmp     rdx, rax                            ; Check against bytes
    jb      .aswp1b

    movzx   r8d, byte [rdi + rax - 0x02]        ; Load, swap and store words
    movzx   r9d, byte [rsi + rax - 0x02]
    mov     byte [rdi + rax - 0x02], r9b
    mov     byte [rsi + rax - 0x02], r8b

    movzx   r8d, byte [rdi + rax - 0x01]
    movzx   r9d, byte [rsi + rax - 0x01]
    mov     byte [rdi + rax - 0x01], r9b
    mov     byte [rsi + rax - 0x01], r8b

    je      .epilogue
    lea     rax, [rax + 0x02]                   ; Advance offset

.aswp1b:
    sub     rax, 0x01
    cmp     rdx, rax                            ; Check against upper bound
    jb      .aswp1b

    movzx   r8d, byte [rdi + rax - 0x01]        ; Load, swap and store bytes
    movzx   r9d, byte [rsi + rax - 0x01]
    mov     byte [rdi + rax - 0x01], r9b
    mov     byte [rsi + rax - 0x01], r8b

.epilogue:
    vzeroupper
    ret

.uswpymmwd:                                     ; Size exactly 32
    vmovdqu ymm0, [rdi]                         ; Load, swap and store ymmwords
    vmovdqu ymm1, [rsi]
    vmovdqu [rdi], ymm1
    vmovdqu [rsi], ymm0
    vzeroupper
    ret

.uswpxmmwd:
    mov     eax, 0x10

    cmp     edx, eax                            ; Check xmmword against upper bound
    jb      .uswpqword

    vmovdqu xmm0, [rdi]                         ; Load, swap and store
    vmovdqu xmm1, [rsi]
    vmovdqu [rdi], xmm1
    vmovdqu [rsi], xmm0

    lea     eax, [eax + 0x10]                   ; Advance

.uswpqword:
    sub     eax, 0x08
    cmp     edx, eax                            ; Check qword against upper bound
    jb      .uswpdword

    vmovq   xmm0, [rdi + rax - 0x08]            ; Load, swap and store
    vmovq   xmm1, [rsi + rax - 0x08]
    vmovq   [rdi + rax - 0x08], xmm1
    vmovq   [rsi + rax - 0x08], xmm0

    lea     eax, [eax + 0x08]                   ; Advance

.uswpdword:
    sub     eax, 0x04
    cmp     edx, eax                            ; Check dword against upper bound
    jb      .uswpwd

    vmovd   xmm0, [rdi + rax - 0x04]            ; Load, swap and store
    vmovd   xmm1, [rsi + rax - 0x04]
    vmovd   [rdi + rax - 0x04], xmm1
    vmovd   [rsi + rax - 0x04], xmm0

    lea     eax, [eax + 0x04]                   ; Advance

.uswpwd:
    sub     eax, 0x02
    cmp     edx, eax                            ; Check word against upper bound
    jb      .uswpbyte

    movzx   r8d, byte [rdi + rax - 0x02]
    movzx   r9d, byte [rsi + rax - 0x02]
    mov     byte [rdi + rax - 0x02], r9b
    mov     byte [rsi + rax - 0x02], r8b

    movzx   r8d, byte [rdi + rax - 0x01]
    movzx   r9d, byte [rsi + rax - 0x01]
    mov     byte [rdi + rax - 0x01], r9b
    mov     byte [rsi + rax - 0x01], r8b

    lea     eax, [eax + 0x02]                   ; Advance

.uswpbyte:
    sub     eax, 0x01
    cmp     edx, eax                            ; Check byte against upper bound
    jb      .uswpdone

    movzx   r8d, byte [rdi + rax - 0x01]        ; Load, swap and store
    movzx   r9d, byte [rsi + rax - 0x01]
    mov     byte [rdi + rax - 0x01], r9b
    mov     byte [rsi + rax - 0x01], r8b

.uswpdone:
    vzeroupper
    ret
