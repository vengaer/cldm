    section .text
    default rel

    global cldm_avx2_strlen

    %include "cldm_config.S"

; AVX2-accelerated strlen
; Params:
;   rdi: Address of string
; Return:
;   rax: Length of string
cldm_avx2_strlen:
    xor     eax, eax                            ; Length
    vpxor   ymm15, ymm15, ymm15

    mov     ecx, edi
    and     ecx, PGSIZE - 1                     ; Mask out page offset
    cmp     ecx, PGSIZE - 0x20                  ; Check whether reading ymmword would cross page boundary
    ja      .pgcross_unaligned

    vmovdqu ymm0, [rdi]                         ; Single unaligned read
    vpcmpeqb    ymm4, ymm0, ymm15
    vptest  ymm4, ymm4
    jnz     .ymmwd0

    lea     rax, [rdi + 0x20]                   ; Align next read to 32 byte boundary
    and     rax, -0x20
    sub     rax, rdi

.cmp128b_prologue:                              ; Compare 4 ymmwords at a time
    lea     rcx, [rdi + rax]
    and     ecx, PGSIZE - 1
    cmp     ecx, PGSIZE - 0x80                  ; Check whether reading 4 ymmwords would cross page boundary
    ja      .pgcross_aligned

.cmp128b:
    vmovdqa ymm0, [rdi + rax + 0x00]            ; Read ymmwords and check for nullbytes in each
    vpcmpeqb    ymm4, ymm0, ymm15
    vmovdqa ymm1, [rdi + rax + 0x20]
    vpcmpeqb    ymm5, ymm1, ymm15
    vmovdqa ymm2, [rdi + rax + 0x40]
    vpcmpeqb    ymm6, ymm2, ymm15
    vmovdqa ymm3, [rdi + rax + 0x60]
    vpcmpeqb    ymm7, ymm3, ymm15
    vpor    ymm8, ymm4, ymm5                    ; Reduce
    vpor    ymm9, ymm6, ymm7
    vpor    ymm10, ymm8, ymm9
    vptest  ymm10, ymm10
    jnz     .nullchk

    add     rax, 0x80                           ; Increment offset
    jmp     .cmp128b_prologue

.nullchk:
    vptest  ymm8, ymm8                          ; Check for null byte in low zmmword
    jnz     .zmmwd0

    vptest  ymm6, ymm6                          ; Check for null byte in third ymmword
    jnz     .ymmwd2

    vpmovmskb   esi, ymm7                       ; Extract bitmask
    tzcnt   edx, esi                            ; Offset in ymmword
    lea     rax, [rax + rdx + 0x60]
    vzeroupper
    ret

.zmmwd0:
    vptest  ymm4, ymm4                          ; Check for null byte in first ymmword
    jnz     .ymmwd0

    vpmovmskb   esi, ymm5                       ; Compute null byte offset
    tzcnt   edx, esi                            ; Offset in ymmword
    lea     rax, [rax + rdx + 0x20]
    vzeroupper
    ret

.ymmwd0:
    vpmovmskb   esi, ymm4                       ; Compute offset of null byte in ymmword
    tzcnt   edx, esi
    add     rax, rdx
    vzeroupper
    ret

.ymmwd2:
    vpmovmskb   esi, ymm6                       ; Null byte in third ymmword
    tzcnt   edx, esi                            ; Offset in ymmword
    lea     rax, [rax + rdx + 0x40]
    vzeroupper
    ret

.pgcross_aligned:                               ; Reading 4 ymmwords would cross page boundary
    vmovdqa ymm0, [rdi + rax]                   ; Load single ymmword
    vpcmpeqb    ymm4, ymm0, ymm15               ; Check for null
    vptest  ymm4, ymm4
    jz      .pgcross_aligned_next

    vpmovmskb   esi, ymm4
    tzcnt   edx, esi
    add     rax, rdx
    vzeroupper
    ret

.pgcross_aligned_next:
    add     rax, 0x20
    lea     rcx, [rdi + rax]                    ; Address of next load
    and     ecx, PGSIZE - 1                     ; Check for page alignment
    test    ecx, ecx
    jz      .cmp128b
    jmp     .pgcross_aligned

.pgcross_unaligned:                             ; Reading single ymmword would cross page boundary
    cmp     ecx, PGSIZE - 0x10                  ; Check whether reading xmmword would cross page boundary
    ja      .pgcross_qword

    vmovdqu xmm0, [rdi]                         ; Load xmmword
    vpcmpeqb    xmm4, xmm0, xmm15               ; Check for null
    vptest  xmm4, xmm4
    jz      .pgcross_unaligned_next

    vpmovmskb   esi, xmm4
    tzcnt   edx, esi
    add     rax, rdx
    vzeroupper
    ret

.pgcross_unaligned_next:
    add     rax, 0x10                           ; Increment size

.pgcross_qword:
    lea     rcx, [rdi + rax]
    and     ecx, PGSIZE - 1                     ; Mask out page offset
    test    ecx, ecx
    jz      .cmp128b
    cmp     ecx, PGSIZE - 0x8                   ; Check whether reading qword would cross boundary
    ja      .pgcross_dword

    vmovq   xmm0, [rdi + rax]                   ; Load qword
    vpcmpeqb    xmm1, xmm0, xmm15               ; Check for null
    vpmovmskb   esi, xmm1                       ; Extract bitmask
    test    esi, 0xff
    jnz     .epi_offset
    add     rax, 0x8                            ; Increment size

.pgcross_dword:
    lea     rcx, [rdi + rax]
    and     ecx, PGSIZE - 1                     ; Mask out page offset
    test    ecx, ecx
    jz      .cmp128b
    cmp     ecx, PGSIZE - 0x4                   ; Check whether reading dword would cross boundary
    ja      .pgcross_word

    vmovd   xmm0, [rdi + rax]                   ; Load dword
    vpcmpeqb    xmm1, xmm0, xmm15               ; Check for null
    vpmovmskb   esi, xmm1                       ; Extract bitmask
    test    esi, 0xf
    jnz     .epi_offset
    add     rax, 0x4                            ; Increment size

.pgcross_word:
    lea     rcx, [rdi + rax]
    and     ecx, PGSIZE - 1                     ; Mask out page offset
    test    ecx, ecx
    jz      .cmp128b
    cmp     ecx, PGSIZE - 0x2                   ; Check whether reading a word would cross boundary
    ja      .pgcross_byte

    movzx   edx, word [rdi + rax]               ; Read word
    vmovd   xmm0, edx                           ; Load into vector register
    vpcmpeqb    xmm1, xmm0, xmm15               ; Check for null
    vpmovmskb   esi, xmm1                       ; Extract bitmask
    test    esi, 0x3
    jnz     .epi_offset
    add     rax, 0x2                            ; Increment size

.pgcross_byte:
    lea     rcx, [rdi + rax]                    ; Check whether address is aligned
    test    ecx, 0x1
    jz      .cmp128b

    cmp     byte [rdi + rax], 0                 ; Check for null
    je      .epilogue

    add     rax, 0x1                            ; Increment size
    jmp     .cmp128b                            ; Aligned to page boundary, process 4 ymmwords

.epi_offset:
    tzcnt   edx, esi                            ; Offset of null byte
    add     rax, rdx
.epilogue:
    vzeroupper
    ret
