    section .text
    default rel

    global cldm_avx2_strcmp

    %include "cldm_config.S"
    %macro  pgchk 2
        %if %2 != 0
            test    r10b, r10b                  ; Check if in a jump chain
            jz      .pgchk_no_chain%1           ; If not, distance to boundary must be computed
            xor     r10d, r10d                  ; In chain, reset indicator
            jmp     .pgchk_masked%1             ; Skip computation of distance to boundary
    .pgchk_no_chain%1:
        %endif

        lea     rax, [rdi + rcx]                ; Address of next read from rdi
        lea     rdx, [rsi + rcx]                ; Address of next read from rsi
        or      eax, edx
        and     eax, PGSIZE - 1                 ; Estimate position in current page

        %if %2 != 0
    .pgchk_masked%1:
        %endif

        cmp     eax, PGSIZE - %1                ; Check if reading %1 bytes risks crossing page
    %endmacro

    %macro ymmdiff 3
        vpmovmskb   eax, %2                     ; Extract bitmask
        not     eax
        tzcnt   edx, eax                        ; Offset of differing byte in ymmword
        lea     rcx, [rcx + rdx]                ; Address offset of differing byte

        vpcmpeqb    ymm11, %1, ymm15            ; Check for null byte
        vpmovmskb   eax, ymm11                  ; Extract bitmask
        tzcnt   r8d, eax                        ; Offset of null byte in ymmword
        cmp     r8d, edx                        ; Check for null byte at index lower than difference
        jb      .epi_eq

        movzx   eax, byte [rdi + rcx + %3]      ; Compare differing bytes
        movzx   edx, byte [rsi + rcx + %3]
        sub     eax, edx                        ; Compute return value
    %endmacro

    %macro alignjmp 0
        lea     rax, [rdi + rcx]                ; Address of next read from rdi
        tzcnt   edx, eax                        ; Check alignment
        cmp     edx, r9d
        cmova   edx, r9d                        ; Clamp offset
        jmp     [r8 + rdx * 0x08]               ; Jump to branch
    %endmacro

; AVX2-accelerated strcmp
; Params:
;   rdi: Address of first string
;   rsi: Address of second string
; Return:
;   eax: = 0 if strings are identical
;        < 0 if [rdi] is less than [rsi],
;        > 0 if [rdi] is greater than [rsi]
cldm_avx2_strcmp:
    xor     ecx, ecx                            ; Offset
    vpcmpeqb    ymm14, ymm14, ymm14
    vpxor   ymm15, ymm15, ymm15

    mov     eax, edi                            ; Compute estimate of position in current page
    or      eax, esi
    and     eax, PGSIZE - 1
    cmp     eax, PGSIZE - 0x20                  ; Check whether reading a ymmword from each string risks crossing page boundary
    ja      .pgcross

    vmovdqu ymm0, [rdi]                         ; Load ymmword
    vpcmpeqb    ymm1, ymm0, [rsi]               ; Compare for equality
    vptest  ymm1, ymm14                         ; Set CF if nand yields zero
    jnc     .ymmrd0_diff

    vpcmpeqb    ymm1, ymm0, ymm15               ; Check for null
    vptest  ymm1, ymm14
    jnz     .epi_eq

    lea     rcx, [rdi + 0x20]                   ; Align next read from rdi to 32-byte boundary
    and     rcx, -0x20
    sub     rcx, rdi

.cmp128b:
    pgchk   0x80, 0
    ja      .pgcross

    vmovdqa ymm0, [rdi + rcx + 0x00]            ; Load 4 ymmwords from first string and compare with second
    vpcmpeqb    ymm4, ymm0, [rsi + rcx + 0x00]
    vmovdqa ymm1, [rdi + rcx + 0x20]
    vpcmpeqb    ymm5, ymm1, [rsi + rcx + 0x20]
    vmovdqa ymm2, [rdi + rcx + 0x40]
    vpcmpeqb    ymm6, ymm2, [rsi + rcx + 0x40]
    vmovdqa ymm3, [rdi + rcx + 0x60]
    vpcmpeqb    ymm7, ymm3, [rsi + rcx + 0x60]

    vpand   ymm8, ymm4, ymm5                    ; Reduce
    vpand   ymm9, ymm6, ymm7
    vpand   ymm13, ymm8, ymm9

    vpcmpeqb    ymm9, ymm0, ymm15               ; Compare against null
    vpcmpeqb    ymm10, ymm1, ymm15
    vpcmpeqb    ymm11, ymm2, ymm15
    vpcmpeqb    ymm12, ymm3, ymm15

    vptest  ymm13, ymm14                        ; Check for differing bytes
    jnc     .dzmmwd_diff

    vpor    ymm8, ymm9, ymm10                   ; Reduce
    vpor    ymm9, ymm11, ymm12
    vpor    ymm10, ymm8, ymm9
    vptest  ymm10, ymm14                        ; Check for null
    jnz     .epi_eq

    add     rcx, 0x80                           ; Advance offset
    jmp     .cmp128b

.dzmmwd_diff:
    vptest  ymm8, ymm14                         ; Check whether diff is in low zmmword
    jnc     .zmmwd_diff

    vptest  ymm6, ymm14                         ; Check whether diff is in third ymmword
    jnc     .ymmwd2_diff

    vpor    ymm8, ymm9, ymm10                   ; Differing byte in fourth ymmword, reduce null checks for first 3 ymmwords
    vpor    ymm9, ymm8, ymm11

    vptest  ymm9, ymm14                         ; Check for null byte
    jnz     .epi_eq

    ymmdiff ymm3, ymm7, 0x60
    vzeroupper
    ret

.ymmwd2_diff:
    vpor    ymm8, ymm9, ymm10                   ; Reduce null checks in first 2 ymmword
    vptest  ymm8, ymm14                         ; Check for null
    jnz     .epi_eq

    ymmdiff ymm2, ymm6, 0x40                    ; Diff in third ymmword
    vzeroupper
    ret

.zmmwd_diff:
    vptest  ymm4, ymm14                         ; Check whether differing byte is in first ymmword
    jnc     .ymmwd0_diff

    vptest  ymm9, ymm14                         ; Check for null in first ymmword
    jnz     .epi_eq

    ymmdiff ymm1, ymm5, 0x20                    ; Diff in second ymmword
    vzeroupper
    ret

.ymmwd0_diff:
    ymmdiff ymm0, ymm4, 0x00                    ; Diff in first ymmword
    vzeroupper
    ret

.epi_eq:
    xor     eax, eax
.epi:
    vzeroupper
    ret

.ymmrd0_diff:
    vpmovmskb   eax, ymm1                       ; Extract bitmask
    not     eax
    tzcnt   edx, eax                            ; Offset of differing byte in ymmword
    lea     rcx, [rcx + rdx]                    ; Address offset of differing byte

    vpcmpeqb    ymm11, ymm0, ymm15              ; Check for null byte
    vpmovmskb   eax, ymm11                      ; Extract bitmask
    tzcnt   r8d, eax                            ; Offset of null byte in ymmword
    cmp     r8d, edx                            ; Check for null byte at index lower than difference
    jb      .epi_eq

    movzx   eax, byte [rdi + rcx]               ; Compare differing bytes
    movzx   edx, byte [rsi + rcx]
    sub     eax, edx                            ; Compute return value
    vzeroupper
    ret

.pgcross:
    section .data
    align   8
.aligntbl:
    dq  .pgcross_byte
    dq  .pgcross_word
    dq  .pgcross_dword
    dq  .pgcross_qword
    dq  .pgcross_xmmword
    dq  .pgcross_ymmword
    dq  .pgcross_zmmword

    section .text

    lea     r8, [.aligntbl]                     ; Load jump table
    mov     r9d, 0x6                            ; For clamping
    xor     r10d, r10d                          ; Used for indicating a jump chain

    alignjmp

.pgcross_byte:
    movzx   eax, byte [rdi + rcx]               ; Read byte from each source
    movzx   edx, byte [rsi + rcx]
    sub     eax, edx                            ; Compare
    jnz     .epi

    add     eax, edx                            ; Check for null
    jz      .epi_eq

    add     rcx, 0x01                           ; Advance offset

    alignjmp

.pgcross_word:
    pgchk   0x02, 1                             ; Check if reading word risks crossing page boundary
    seta    r10b                                ; If so, avoid recomputing page position
    ja      .pgcross_byte

    movzx   eax, byte [rdi + rcx]               ; Read first byte of word from each string
    movzx   edx, byte [rsi + rcx]

    sub     eax, edx                            ; Compare
    jnz     .epi

    add     eax, edx                            ; Check for null
    jz      .epi

    movzx   eax, byte [rdi + rcx + 0x01]        ; Read second byte of word from each string
    movzx   edx, byte [rsi + rcx + 0x01]

    sub     eax, edx                            ; Compare
    jnz     .epi

    add     eax, edx                            ; Check for null
    jz      .epi

    add     ecx, 0x02                           ; Advance offset

    alignjmp

.pgcross_dword:
    pgchk   0x04, 1                             ; Check if reading dword risks crossing page boundary
    seta    r10b                                ; If so, avoid recomputing page position
    ja      .pgcross_word

    vmovd   xmm0, [rdi + rcx]                   ; Read dwords
    vmovd   xmm2, [rsi + rcx]

    vpcmpeqb    xmm1, xmm0, xmm2                ; Compare for equality
    vptest  xmm1, xmm14
    jnc     .pgcross_diff

    vpcmpeqb    xmm2, xmm0, xmm15               ; Check for null
    vpmovmskb   eax, xmm2
    tzcnt   edx, eax
    cmp     edx, 0x04                           ; Null byte in low 4 bytes
    jb      .epi_eq

    add     ecx, 0x04                           ; Advance offset

    alignjmp

.pgcross_qword:
    pgchk   0x08, 1                             ; Check if reading qword risks crossing page boundary
    seta    r10b                                ; If so, avoid recomputing page position
    ja      .pgcross_dword

    vmovq   xmm0, [rdi + rcx]                   ; Read qword from each string
    vmovq   xmm2, [rsi + rcx]

    vpcmpeqb    xmm1, xmm0, xmm2                ; Compare for equality
    vptest  xmm1, xmm14
    jnc     .pgcross_diff

    vpcmpeqb    xmm2, xmm0, xmm15               ; Check for null
    vpmovmskb   eax, xmm2
    tzcnt   edx, eax
    cmp     edx, 0x08                           ; Null byte in low 8 bytes
    jb      .epi_eq

    add     ecx, 0x08                           ; Advance offset

    alignjmp

.pgcross_xmmword:
    pgchk   0x10, 1                             ; Check if reading xmmword risks crossing page boundary
    seta    r10b                                ; If so, avoid recomputing page position
    ja      .pgcross_qword

    vmovdqu xmm0, [rdi + rcx]                   ; Check for differing bytes
    vpcmpeqb    xmm1, xmm0, [rsi + rcx]         ; Compare for equality
    vptest  xmm1, xmm14
    jnc     .pgcross_diff

    vpcmpeqb    xmm1, xmm0, xmm15               ; Check for null byte
    vptest  xmm1, xmm14
    jnz     .epi_eq

    add     ecx, 0x10                           ; Advance offset

    alignjmp

.pgcross_ymmword:
    pgchk   0x20, 1                             ; Check if reading ymmword risks crossing page boundary
    seta    r10b                                ; If so, avoid recomputing page position
    ja      .pgcross_xmmword

    vmovdqa ymm0, [rdi + rcx]                   ; Load single ymmword
    vpcmpeqb    ymm4, ymm0, [rsi + rcx]         ; Compare for equality
    vptest  ymm4, ymm14
    jnc     .ymmwd0_diff

    vpcmpeqb    ymm4, ymm0, ymm15               ; Compare against null
    vptest  ymm4, ymm14
    jnz     .epi_eq

    lea     rcx, [rcx + 0x20]                   ; Advance offset

    alignjmp

.pgcross_zmmword:
    pgchk   0x40, 1                             ; Check if reading 2 ymmwords risks crossing page boundary
    seta    r10b                                ; If so, avoid recomputing page position
    ja      .pgcross_ymmword

    vmovdqa ymm0, [rdi + rcx + 0x00]            ; Load and compare 2 ymmwords from each string
    vpcmpeqb    ymm4, ymm0, [rsi + rcx + 0x00]
    vmovdqa ymm1, [rdi + rcx + 0x20]
    vpcmpeqb    ymm5, ymm1, [rsi + rcx + 0x20]

    vpand   ymm13, ymm4, ymm5                   ; Reduce

    vpcmpeqb    ymm9, ymm0, ymm15               ; Compare against null
    vpcmpeqb    ymm10, ymm1, ymm15

    vptest  ymm13, ymm14                        ; Check for differing bytes
    jnc     .zmmwd_diff

    vpor    ymm8, ymm9, ymm10                   ; Reduce
    vptest  ymm8, ymm14                         ; Check for null
    jnz     .epi_eq

    add     rcx, 0x40                           ; Advance offset
    jmp     .cmp128b

.pgcross_diff:
    vpmovmskb   eax, xmm1                       ; Extract bitmask
    not     eax
    tzcnt   edx, eax                            ; Offset of differing byte in xmmword
    lea     rcx, [rcx + rdx]

    vpcmpeqb    xmm11, xmm0, xmm15              ; Check for null byte
    vpmovmskb   eax, xmm11                      ; Extract bitmask
    tzcnt   r8d, eax                            ; Offset of null byte in ymmword
    cmp     r8d, edx                            ; Check for null byte at index lower than difference
    jb      .epi_eq

    movzx   eax, byte [rdi + rcx]               ; Compare differing bytes
    movzx   edx, byte [rsi + rcx]
    sub     eax, edx                            ; Compute return value
    vzeroupper
    ret
