    section .text
    default rel

    global cldm_avx2_strrchr

    %include "cldm_config.S"

    %macro vchk 4
        vpmovmskb   r8d, %1
        vpmovmskb   r9d, %2
        tzcnt   ecx, r9d                        ; Index of null byte in ymmword
        jc      .vchk_masked%4                  ; Mask only if null byte is found

        mov     r9d, 0x01
        shl     r9d, cl
        lea     ecx, [r9d * 0x02 - 0x01]        ; Construct bitmask
        and     r8d, ecx                        ; Mask out potential matches after null byte

    .vchk_masked%4:
        lzcnt   r9d, r8d                        ; Index of last matching character
        not     r9d                             ; Multiply by -1 and subtract 1
        movsx   r8, r9d                         ; Sign extend
        lea     r8, [r8 + rdx]
        lea     r9, [rdi + r8 + 0x20 + %3]
        cmovnc  rax, r9                         ; Store address if there was a match

        vptest  %2, ymm14
        jnz     .epi
    %endmacro

; AVX2-accelerated strrchr
; Params:
;   rdi: Address of string
;   esi: The byte to search for
; Return:
;   rax: Address of last esi found in [rdi],
;        or 0 if none is found
cldm_avx2_strrchr:
    xor     eax, eax                            ; Return value
    xor     edx, edx                            ; Offset
    vpxor   ymm15, ymm15, ymm15
    vpcmpeqb    ymm14, ymm14, ymm14

    vpxor   xmm2, xmm2, xmm2                    ; Shuffle mask
    vmovd   xmm1, esi                           ; Insert dword
    vpshufb xmm0, xmm1, xmm2                    ; Broadcast 0th byte
    vinserti128 ymm13, ymm0, xmm0, 0x1          ; Copy low xmmword to high

    mov     ecx, edi                            ; Estimate position in current page
    and     ecx, PGSIZE - 1
    cmp     ecx, PGSIZE - 0x20
    ja      .pgcross_xmmwd0

    vmovdqu ymm0, [rdi]                         ; Load ymmword
    vpcmpeqb    ymm1, ymm0, ymm13               ; Compare against char
    vpcmpeqb    ymm2, ymm0, ymm15               ; Compare against null

    vpor    ymm3, ymm1, ymm2
    vptest  ymm3, ymm14
    jz      .chk128b_align

    vchk    ymm1, ymm2, 0x00, 0

.chk128b_align:
    lea     rdx, [rdi + 0x20]                   ; Align next read to 32 byte boundary
    and     rdx, -0x20
    sub     rdx, rdi

.chk128b_prologue:
    lea     rcx, [rdi + rdx]                    ; Check if reading 4 ymmwords would cross page boundary
    and     ecx, PGSIZE - 1
    cmp     ecx, PGSIZE - 0x80
    ja      .pgcross_aligned

.chk128b:
    vmovdqa ymm0, [rdi + rdx + 0x00]            ; Load 4 ymmwords and compare against both char and null
    vpcmpeqb    ymm4, ymm0, ymm13
    vpcmpeqb    ymm8, ymm0, ymm15
    vmovdqa ymm1, [rdi + rdx + 0x20]
    vpcmpeqb    ymm5, ymm1, ymm13
    vpcmpeqb    ymm9, ymm1, ymm15
    vmovdqa ymm2, [rdi + rdx + 0x40]
    vpcmpeqb    ymm6, ymm2, ymm13
    vpcmpeqb    ymm10, ymm2, ymm15
    vmovdqa ymm3, [rdi + rdx + 0x60]
    vpcmpeqb    ymm7, ymm3, ymm13
    vpcmpeqb    ymm11, ymm3, ymm15

    vpor    ymm0, ymm4, ymm8                    ; Reduce
    vpor    ymm1, ymm5, ymm9
    vpor    ymm2, ymm6, ymm10
    vpor    ymm3, ymm7, ymm11
    vpor    ymm12, ymm0, ymm1
    vpor    ymm12, ymm2, ymm12
    vpor    ymm12, ymm3, ymm12

    vptest  ymm12, ymm14                        ; Check if there is either a null or a matching byte
    jnz     .chk_ymmwd0

.chk128b_epilogue:
    lea     rdx, [rdx + 0x80]                   ; Advance offset
    jmp     .chk128b_prologue

.chk_ymmwd0:
    vptest  ymm0, ymm14                         ; Check for set byte
    jz      .chk_ymmwd1

    vchk    ymm4, ymm8, 0x00, 1

.chk_ymmwd1:
    vptest  ymm1, ymm14                         ; Check for set byte
    jz      .chk_ymmwd2

    vchk    ymm5, ymm9, 0x20, 2

.chk_ymmwd2:
    vptest  ymm2, ymm14                         ; Check for set byte
    jz      .chk_ymmwd3

    vchk    ymm6, ymm10, 0x40, 3

.chk_ymmwd3:
    vptest  ymm3, ymm14                         ; Check for set byte
    jz      .chk128b_epilogue

    vchk    ymm7, ymm11, 0x60, 4
    jmp     .chk128b_epilogue

.pgcross_aligned:
    vmovdqa ymm0, [rdi + rdx]                   ; Process single ymmword at a time
    vpcmpeqb    ymm4, ymm0, ymm13               ; Compare against char
    vpcmpeqb    ymm8, ymm0, ymm15               ; Compare against null

    vpor    ymm0, ymm4, ymm8                    ; Reduce
    vptest  ymm0, ymm14                         ; Check if either null or matching bytes
    jz      .pgcross_aligned_boundchk

    vchk    ymm4, ymm8, 0x00, 5

.pgcross_aligned_boundchk:
    lea     rdx, [rdx + 0x20]                   ; Check if on page boundary
    lea     rcx, [rdi + rdx]
    and     ecx, PGSIZE - 1
    test    ecx, ecx
    jz      .chk128b                            ; On boundary, safe to read 4 ymmwords again
    jmp     .pgcross_aligned

.pgcross_xmmwd0:
    cmp     ecx, PGSIZE - 0x10                  ; Check if reading an xmmword would cross page boundary
    ja      .pgcross_qword0

    vmovdqu xmm0, [rdi]
    vpcmpeqb    xmm4, xmm0, xmm13               ; Compare against char
    vpcmpeqb    xmm8, xmm0, xmm15               ; Compare against null

    lea     rdx, [rdi + 0x10]                   ; Align next read to 16-byte boundary
    and     rdx, -0x10
    sub     rdx, rdi

    vpor    xmm0, xmm4, xmm8                    ; Reduce

    mov     r10d, ecx                           ; Store page position

    vptest  xmm0, xmm14                         ; Check for null and matching bytes
    jz      .pgcross_xmmwd1

    vpmovmskb   r8d, xmm4                       ; Extract char mask
    vpmovmskb   r9d, xmm8                       ; Extract null mask

    tzcnt   ecx, r9d                            ; Index of first null byte
    jc      .pgcross_xmmwd0_masked              ; If no null byte, no need to mask

    mov     r9d, 0x01                           ; Mask out char matches after null byte
    shl     r9d, cl
    lea     ecx, [r9d * 0x02 - 0x01]            ; Construct bitmask
    and     r8d, ecx

.pgcross_xmmwd0_masked:
    shl     r8d, 0x10                           ; Shift out bits corresponding to zeroed lanes
    lzcnt   r9d, r8d                            ; Leading zero bits
    not     r9d                                 ; Multiply by -1 and subtract 1
    movsx   r8, r9d                             ; Sign extend
    lea     r9, [rdi + r8 + 0x10]               ; Address of last match
    cmovnc  rax, r9                             ; Store if there was a matching char

    vptest  xmm8, xmm14                         ; Done if there is a null byte in read xmmword
    jnz     .epi

.pgcross_xmmwd1:
    lea     r10d, [r10d + edx]                  ; Check if aligned to page boundary
    cmp     r10d, PGSIZE
    je      .chk128b

    vmovdqa xmm0, [rdi + rdx]
    vpcmpeqb    xmm4, xmm0, xmm13               ; Compare against char
    vpcmpeqb    xmm8, xmm0, xmm15               ; Compare against null

    lea     rdx, [rdx + 0x10]                   ; Advance offset

    vpor    xmm0, xmm4, xmm8                    ; Reduce

    vptest  xmm0, xmm14                         ; Check for null and matching bytes
    jz      .chk128b

    vpmovmskb   r8d, xmm4                       ; Extract char mask
    vpmovmskb   r9d, xmm8                       ; Extract null mask

    tzcnt   ecx, r9d                            ; Index of first null byte
    jc      .pgcross_xmmwd1_masked              ; No need to mask if there is no null byte

    mov     r9d, 0x01                           ; Mask out matches after null byte
    shl     r9d, cl
    lea     ecx, [r9d * 0x02 - 0x01]            ; Construct bitmask
    and     r8d, ecx

.pgcross_xmmwd1_masked:
    shl     r8d, 0x10                           ; Shift out bits corresponding to zeroed lanes
    lzcnt   r9d, r8d                            ; Leading zero bits
    not     r9d                                 ; Multiply by -1 and subtract 1
    movsx   r8, r9d                             ; Sign extend
    lea     r9, [r8 + rdx]                      ; Offset of last matching char relative rdi
    lea     r8, [rdi + r9]                      ; Address of last matching char
    cmovnc  rax, r8

    vptest  xmm8, xmm14                         ; Check if done
    jnz     .epi

    jmp     .chk128b

.pgcross_qword0:
    cmp     ecx, PGSIZE - 0x08                  ; Check if reading qword would cross page boundary
    ja      .pgcross_dword0

    vmovq   xmm0, [rdi]                         ; Load qword
    vpcmpeqb    xmm4, xmm0, xmm13               ; Compare against char
    vpcmpeqb    xmm8, xmm0, xmm15               ; Compare against null

    lea     rdx, [rdi + 0x08]                   ; Align next read to 8 byte boundary
    and     rdx, -0x08
    sub     rdx, rdi

    mov     r11d, ecx                           ; Store page position

    xor     r10d, r10d                          ; Null indicator

    vpor    xmm0, xmm4, xmm8                    ; Reduce
    vpmovmskb   ecx, xmm0                       ; Extract bitmask
    test    ecx, 0xff                           ; Check for matching lane in low qword
    jz      .pgcross_qword1

    vpmovmskb   r8d, xmm4                       ; Extract char mask
    vpmovmskb   r9d, xmm8                       ; Extract null mask

    and     r9d, 0xff                           ; Mask out bits corresponding to zeroed lanes
    tzcnt   ecx, r9d                            ; Index of null byte in qword
    setnc   r10b                                ; Indicate presence of null byte
    jc      .pgcross_qword0_masked

    mov     r9d, 0x01                           ; Mask out potential matches after null byte
    shl     r9d, cl
    lea     ecx, [r9d * 0x02 - 0x01]            ; Construct bitmask
    and     r8d, ecx

.pgcross_qword0_masked:
    shl     r8d, 0x18                           ; Shift out bits corresponding to zeroed lanes
    lzcnt   r9d, r8d                            ; Index of last matching character
    not     r9                                  ; Multiply by -1 and subtract 1
    lea     r8, [rdi + r9 + 0x08]               ; Address of last matching char
    cmovnc  rax, r8

    test    r10d, r10d                          ; Check if there was a null byte
    jnz     .epi

.pgcross_qword1:
    lea     r11d, [r11d + edx]                  ; Check if aligned to page boundary
    cmp     r11d, PGSIZE
    je      .chk128b

    vmovq   xmm0, [rdi + rdx]
    vpcmpeqb    xmm4, xmm0, xmm13               ; Check against char
    vpcmpeqb    xmm8, xmm0, xmm15               ; Check against null

    lea     rdx, [rdx + 0x08]                   ; Advance offset

    vpor    xmm0, xmm4, xmm8                    ; Reduce
    vpmovmskb   ecx, xmm0                       ; Extract bitmask
    test    ecx, 0xff                           ; Check for matching lane in low qword
    jz      .chk128b

    vpmovmskb   r8d, xmm4                       ; Extract char mask
    vpmovmskb   r9d, xmm8                       ; Extract null mask

    and     r9d, 0xff                           ; Mask out bits corresponding to zeroed lanes
    tzcnt   ecx, r9d                            ; Index of null byte in qword
    setnc   r10b                                ; Indicate presence of null byte
    jc      .pgcross_qword1_masked

    mov     r9d, 0x01                           ; Mask out potential matches after null byte
    shl     r9d, cl
    lea     ecx, [r9d * 0x02 - 0x01]            ; Construct bitmask
    and     r8d, ecx

.pgcross_qword1_masked:
    shl     r8d, 0x18                           ; Shift out bits corresponding to zeroed lanes
    lzcnt   r9d, r8d                            ; Index of last matching character
    not     r9d                                 ; Multiply by -1 and subtract 1
    movsx   r8, r9d                             ; Sign extend
    lea     r9, [r8 + rdx]                      ; Offset of last matching char relative rdi
    lea     r8, [rdi + r9]                      ; Address of last matching char
    cmovnc  rax, r8

    test    r10d, r10d                          ; Check if there was a null byte
    jnz     .epi

    jmp     .chk128b

.pgcross_dword0:
    cmp     ecx, PGSIZE - 0x04                  ; Check if reading dword would cross page boundary
    ja      .pgcross_word

    vmovd   xmm0, [rdi]                         ; Load qword
    vpcmpeqb    xmm4, xmm0, xmm13               ; Compare against char
    vpcmpeqb    xmm8, xmm0, xmm15               ; Compare against null

    lea     rdx, [rdi + 0x04]                   ; Align next read to 4 byte boundary
    and     rdx, -0x04
    sub     rdx, rdi

    vpor    xmm0, xmm4, xmm8                    ; Reduce

    mov     r11d, ecx                           ; Store page position

    xor     r10d, r10d                          ; Null indicator

    vpmovmskb   ecx, xmm0                       ; Extract bitmask
    test    ecx, 0x0f                           ; Check for matching lane in low dword
    jz      .pgcross_dword1

    vpmovmskb   r8d, xmm4                       ; Extract char bitmask
    vpmovmskb   r9d, xmm8                       ; Extract null bitmask

    and     r9d, 0x0f                           ; Mask out bits corresponding to zeroed lanes
    tzcnt   ecx, r9d                            ; Index of null byte in dword
    setnc   r10b                                ; Indicate presence of null byte
    jc      .pgcross_dword0_masked

    mov     r9d, 0x01
    shl     r9d, cl
    lea     ecx, [r9d * 0x02 - 0x01]            ; Construct bitmask
    and     r8d, ecx

.pgcross_dword0_masked:
    shl     r8d, 0x1c                           ; Shift out bits corresponding to zeroed lanes
    lzcnt   r9d, r8d                            ; 4 - Index of last matching character
    not     r9d                                 ; Multiply by -1 and subtract 1
    movsx   r8, r9d                             ; Sign extend
    lea     r9, [rdi + r8 + 0x04]               ; Address of last matching char
    cmovnc  rax, r9

    test    r10d, r10d                          ; Check for null byte
    jnz     .epi

.pgcross_dword1:
    lea     r11d, [r11d + edx]                  ; Check if aligned to page boundary
    cmp     r11d, PGSIZE
    je      .chk128b

    vmovd   xmm0, [rdi + rdx]
    vpcmpeqb    xmm4, xmm0, xmm13               ; Compare against char
    vpcmpeqb    xmm8, xmm0, xmm15               ; Compare against null

    lea     rdx, [rdx + 0x04]                   ; Advance offset

    vpor    xmm0, xmm4, xmm8                    ; Reduce
    vpmovmskb   ecx, xmm0                       ; Extract bitmask
    test    ecx, 0x0f                           ; Check for matching lane in low dword
    jz      .chk128b

    vpmovmskb   r8d, xmm4                       ; Extract char mask
    vpmovmskb   r9d, xmm8                       ; Extract null mask

    and     r9d, 0x0f                           ; Mask out bits corresponding to zeroed lanes
    tzcnt   ecx, r9d                            ; Index if null byte in dword
    setnc   r10b                                ; Indicate presence of null byte
    jc      .pgcross_dword1_masked

    mov     r9d, 0x01                           ; Mask out potential matches after null byte
    shl     r9d, cl
    lea     ecx, [r9d * 0x02 - 0x01]            ; Construct bitmask
    and     r8d, ecx

.pgcross_dword1_masked:
    shl     r8d, 0x1c                           ; Shift out bits corresponding to zeroed lanes
    lzcnt   r9d, r8d                            ; Index of last matching character relative msb
    not     r9d                                 ; Multiply by -1 and subtract 1
    movsx   r8, r9d                             ; Sign extend
    lea     r9, [r8 + rdx]                      ; Offset of last matching char relative rdi
    lea     r8, [rdi + r9]                      ; Address of last matching char
    cmovnc  rax, r8

    test    r10d, r10d                          ; Check if done
    jnz     .epi

    jmp     .chk128b

.pgcross_word:
    cmp     ecx, PGSIZE - 0x02                  ; Check if reading word would cross page boundary
    ja      .pgcross_byte

    movzx   r8d, byte [rdi]                     ; Load
    cmp     r8d, esi                            ; Compare against char
    cmove   rax, rdi                            ; If match, store address

    test    r8d, r8d                            ; Check for null
    jz      .epi

.pgcross_word_byte1:
    lea     r9, [rdi + 0x01]                    ; Address of second byte
    movzx   r8d, byte [r9]                      ; Load
    cmp     r8d, esi                            ; Compare against char
    cmove   rax, r9                             ; If match, store address

    test    r8d, r8d                            ; Check for null
    jz      .epi

    lea     rdx, [rdx + 0x02]

.pgcross_byte:
    lea     rcx, [rdi + rdx]                    ; Compute position in current page
    and     ecx, PGSIZE - 1
    test    ecx, ecx                            ; Check if on page boundary
    jz      .chk128b

    lea     r9, [rdi + rdx]                     ; Address of byte to load
    movzx   r8d, byte [r9]                      ; Load
    cmp     r8d, esi                            ; Compare against char
    cmove   rax, r9                             ; If match, store address

    test    r8d, r8d                            ; Check for null
    jz      .epi

    lea     rdx, [rdx + 0x01]

    jmp     .chk128b

.epi:
    vzeroupper
    ret
