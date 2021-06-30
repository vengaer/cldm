    section .text

    default rel

    global cldm_avx2_strscpy

    %include "cldm_config.S"

    %define E2BIG 7

; AVX2-accelerated string copy routine.
; Provided that the destination buffer can
; hold at least 1 byte, the result is
; guaranteed to be null terminated. If
; source is larger than destination, rdx - 1
; bytes of source are written to the
; destination.
;
; Source and destination may not overlap.
;
; Accesses beyond the null-terminator in
; source are, by design, likely to occur.
; No accesses, neither read nor writes,
; risk hitting the guard page.
;
; Params:
;   rdi: address of destination
;   rsi: address of source
;   rdx: size of destination
; Return:
;   rax: Size of the string written to
;        [rdx], or -7 if the length of
;        [rsi] is greater than rdx
cldm_avx2_strscpy:
    test    rdx, rdx
    jz      .epi_empty

    xor     eax, eax                            ; Offset

    mov     ecx, esi                            ; Check whether copying a single ymmword would cross page boudary
    and     ecx, PGSIZE - 1
    cmp     ecx, PGSIZE - 0x20
    ja      .pgcross_unaligned

    cmp     rdx, 0x20
    jb      .bufend_unaligned

    vpxor   ymm15, ymm15

    vmovdqu ymm0, [rsi]                         ; Load first ymmword
    vpcmpeqb    ymm4, ymm0, ymm15               ; Compare to null
    vmovdqu [rdi], ymm0

    vptest  ymm4, ymm4                          ; Check if there was a null byte
    jz      .align32b

    vpmovmskb   ecx, ymm4                       ; Extract bitmask
    tzcnt   eax, ecx                            ; Length is index of first null byte
    vzeroupper
    ret

.align32b:
    lea     rax, [rsi + 0x20]                   ; Align reads to 32-byte boundary
    and     rax, -0x20
    sub     rax, rsi

.cpy128b_pgchk:
    lea     rcx, [rsi + rax]                    ; Check for risk of crossing page boundary
    and     ecx, PGSIZE - 1
    cmp     ecx, PGSIZE - 0x80
    ja      .pgcross_aligned

.cpy128b:
    lea     rcx, [rax + 0x80]                   ; Check for end of buffer
    cmp     rcx, rdx
    ja      .bufend_aligned

    vmovdqa ymm0, [rsi + rax + 0x00]            ; Load 4 ymmwords and compare to null
    vpcmpeqb    ymm4, ymm0, ymm15
    vmovdqa ymm1, [rsi + rax + 0x20]
    vpcmpeqb    ymm5, ymm1, ymm15
    vmovdqa ymm2, [rsi + rax + 0x40]
    vpcmpeqb    ymm6, ymm2, ymm15
    vmovdqa ymm3, [rsi + rax + 0x60]
    vpcmpeqb    ymm7, ymm3, ymm15

    vpor    ymm8, ymm4, ymm5                    ; Reduce
    vpor    ymm9, ymm6, ymm7
    vpor    ymm10, ymm8, ymm9

    vmovdqu [rdi + rax + 0x00], ymm0            ; First ymmword always written back

    vptest  ymm10, ymm10                        ; Check if a null byte was encountered
    jnz     .null128b

    vmovdqu [rdi + rax + 0x20], ymm1            ; No null byte, write remaining ymmwords
    vmovdqu [rdi + rax + 0x40], ymm2
    vmovdqu [rdi + rax + 0x60], ymm3

    lea     rax, [rax + 0x80]                   ; Advance offset
    jmp     .cpy128b_pgchk

.null128b:
    vptest  ymm8, ymm8                          ; Check if null byte is in low zmmword
    jnz     .null64b

    vmovdqu [rdi + rax + 0x20], ymm1            ; Null in either third or fourth ymmwords, at least 3 ymmwords written
    vmovdqu [rdi + rax + 0x40], ymm2

    vptest  ymm6, ymm6                          ; Check for null in third ymmword
    jnz     .null96b

    vmovdqu [rdi + rax + 0x60], ymm3            ; Null byte in fourth ymmword, store

    vpmovmskb   ecx, ymm7                       ; Compute offset of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8 + 0x60]              ; Advance offset
    vzeroupper
    ret

.null64b:
    vptest  ymm4, ymm4                          ; Check if nullbyte in first ymmword
    jnz     .null32b

    vmovdqu [rdi + rax + 0x20], ymm1            ; Null byte in second ymmword, store

    vpmovmskb   ecx, ymm5                       ; Compute offset of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8 + 0x20]              ; Advance offset
    vzeroupper
    ret

.null32b:
    vpmovmskb   ecx, ymm4                       ; Compute index of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8]                     ; Add null byte offset
    vzeroupper
    ret

.null96b:
    vpmovmskb   ecx, ymm6                       ; Compute index of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8 + 0x40]              ; Advance offset
    vzeroupper
    ret

.pgcross_aligned:
    lea     rcx, [rax + 0x20]                   ; Check against end of buffer
    cmp     rcx, rdx
    ja      .pgcross_aligned_bufend

    vmovdqa ymm0, [rsi + rax]                   ; Load, compare to null and store
    vpcmpeqb    ymm4, ymm0, ymm15
    vmovdqu [rdi + rax], ymm0

    lea     rax, [rax + 0x20]                   ; Advance offset

    vptest  ymm4, ymm4                          ; Check whether null byte was encountered
    jnz     .pgcross_aligned_epi

    lea     rcx, [rsi + rax]                    ; Check if aligned to page boundary
    and     ecx, PGSIZE - 1
    jz      .cpy128b

    jmp     .pgcross_aligned

.pgcross_aligned_bufend:
    mov     rcx, rdx                            ; Compute number of bytes to retract offse
    sub     rcx, rax
    neg     rcx
    lea     r10d, [rcx + 0x20]
    jmp     .bufend32b

.pgcross_aligned_epi:
    vpmovmskb   ecx, ymm4                       ; Compute offset of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8 - 0x20]              ; Compute actual length contribution of ymmword
    vzeroupper
    ret


.bufend_aligned:
    section .data
    align   8
.endtbl:
    dq  .bufend32b
    dq  .bufend64b
    dq  .bufend96b
    dq  .bufend128b

    section .text

    lea     r8, [.endtbl]                       ; Load jump table
    mov     rcx, rdx                            ; Compute number of remaining bytes in buffer
    sub     rcx, rax

    mov     r9d, ecx                            ; Compute offset
    shr     r9d, 0x05                           ; Divide by 32

    and     ecx, 0x1f                           ; Compute number of bytes to retract
    neg     ecx
    lea     r10d, [ecx + 0x20]

    jmp     [r8 + r9 * 8]                       ; Jump to branch

.bufend32b:
    sub     rax, r10                            ; Set up offset for final load

    vmovdqu ymm0, [rsi + rax]                   ; Load, compare and store last ymmword
    vpcmpeqb    ymm4, ymm0, ymm15
    vmovdqu [rdi + rax], ymm0

    vptest  ymm4, ymm4                          ; Check if a null byte was encountered
    jz      .epi_ovf_clear

    vpmovmskb   ecx, ymm4                       ; Compute offset of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8]                     ; Final length
    vzeroupper
    ret

.bufend64b:
    vmovdqa ymm0, [rsi + rax]                   ; Load, compare and store first ymmword
    vpcmpeqb    ymm4, ymm0, ymm15
    vmovdqu [rdi + rax], ymm0

    vptest  ymm4, ymm4                          ; Check if null byte was encountered
    jz      .bufend64b_ymmwd1

    vpmovmskb   ecx, ymm4                       ; Compute offset of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8]                     ; Final length
    vzeroupper
    ret

.bufend64b_ymmwd1:
    lea     rax, [rax + 0x20]                   ; Set up offset for final load
    sub     rax, r10

    vmovdqu ymm1, [rsi + rax]                   ; Load, compare and store final ymmword
    vpcmpeqb    ymm5, ymm1, ymm15
    vmovdqu [rdi + rax], ymm1

    vptest  ymm5, ymm5                          ; Check if null byte was encountered
    jz      .epi_ovf_clear

    vpmovmskb   ecx, ymm5                       ; Compute offset of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8]                     ; Final length
    vzeroupper
    ret

.bufend96b:
    vmovdqa ymm0, [rsi + rax]                   ; Load, compare and store first ymmword
    vpcmpeqb    ymm4, ymm0, ymm15
    vmovdqu [rdi + rax], ymm0

    vptest  ymm4, ymm4                          ; Check if a null byte was encountered
    jz      .bufend96b_ymmwd1

    vpmovmskb   ecx, ymm4                       ; Offset of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8]                     ; Final length
    vzeroupper
    ret

.bufend96b_ymmwd1:
    vmovdqa ymm1, [rsi + rax + 0x20]            ; Load, compare and store second ymmword
    vpcmpeqb    ymm5, ymm1, ymm15
    vmovdqu [rdi + rax + 0x20], ymm1

    vptest  ymm5, ymm5                          ; Check whether null byte was encountered
    jz      .bufend96b_ymmwd2

    vpmovmskb   ecx, ymm5                       ; Offset of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8 + 0x20]              ; Final length
    vzeroupper
    ret

.bufend96b_ymmwd2:
    lea     rax, [rax + 0x40]                   ; Set up offset for final load
    sub     rax, r10

    vmovdqu ymm2, [rsi + rax]                   ; Load, compare and store final ymmword
    vpcmpeqb    ymm6, ymm2, ymm15
    vmovdqu [rdi + rax], ymm2

    vptest  ymm6, ymm6                          ; Check if null byte was encountered
    jz      .epi_ovf_clear

    vpmovmskb   ecx, ymm6                       ; Offset of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8]                     ; Final length
    ret

.bufend128b:
    vmovdqa ymm0, [rsi + rax]                   ; Load, compare and store first ymmword
    vpcmpeqb    ymm4, ymm0, ymm15
    vmovdqu [rdi + rax], ymm0

    vptest  ymm4, ymm4                          ; Check if null byte was encountered
    jz      .bufend128b_ymmwd1

    vpmovmskb   ecx, ymm4                       ; Offset of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8]                     ; Final length
    vzeroupper
    ret

.bufend128b_ymmwd1:
    vmovdqa ymm1, [rsi + rax + 0x20]            ; Load, compare and store second ymmword
    vpcmpeqb    ymm5, ymm1, ymm15
    vmovdqu [rdi + rax + 0x20], ymm1

    vptest  ymm5, ymm5                          ; Check whether null byte was encountered
    jz      .bufend128b_ymmwd2

    vpmovmskb   ecx, ymm5                       ; Offset of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8 + 0x20]              ; Final length
    vzeroupper
    ret

.bufend128b_ymmwd2:
    vmovdqa ymm2, [rsi + rax + 0x40]            ; Load, compare and store third ymmword
    vpcmpeqb    ymm6, ymm2, ymm15
    vmovdqu [rdi + rax + 0x40], ymm2

    vptest  ymm6, ymm6                          ; Check whether null byte was encountered
    jz      .bufend128b_ymmwd3

    vpmovmskb   ecx, ymm6                       ; Offset of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8 + 0x40]              ; Final length
    vzeroupper
    ret

.bufend128b_ymmwd3:
    lea     rax, [rax + 0x60]                   ; Set up offset for final load
    sub     rax, r10

    vmovdqu ymm3, [rsi + rax]                   ; Load, compare and store final ymmword
    vpcmpeqb    ymm7, ymm3, ymm15
    vmovdqu [rdi + rax], ymm3

    vptest  ymm7, ymm7                          ; Check whether null byte was encountered
    jz      .epi_ovf_clear

    vpmovmskb   ecx, ymm7                       ; Offset of null byte in ymmword
    tzcnt   r8d, ecx
    lea     rax, [rax + r8]                     ; Final length
    vzeroupper
    ret

.pgcross_unaligned:
    neg     ecx
    lea     r9d, [ecx + PGSIZE]
    cmp     r9, rdx                            ; Check if buffer ends before memory page does
    jnb     .bufend_unaligned

    section .data
    align   8
.pgcrosstbl:
    dq  .pgcross1b
    dq  .pgcross2b
    dq  .pgcross4b
    dq  .pgcross8b
    dq  .pgcross16b

    section .text

    lea     r8, [.pgcrosstbl]                   ; Load jump table
    lzcnt   ecx, r9d                            ; Number of leading zero bits
    neg     rcx
    jmp     [r8 + (rcx + 0x1f) * 8]             ; Jump to branch, rcx + 0x1f -> most significant set bit in r9d, zero-indexed

.pgcross1b:
    movzx   ecx, byte [rsi]                     ; Copy byte
    mov     byte [rdi], cl

    test    ecx, ecx
    setnz   al
    jnz     .cpy128b                            ; Aligned to page boundary
    ret

.pgcross2b:
    movzx   eax, byte [rsi + 0x00]              ; Copy first 2 bytes
    mov     byte [rdi + 0x00], al
    movzx   ecx, byte [rsi + 0x01]
    mov     byte [rdi + 0x01], cl

    xor     r11d, r11d
    mov     r10d, 0xff
    test    eax, eax
    cmovnz  r11d, r10d

    test    ecx, r11d                           ; Check whether either byte was null
    jnz     .pgcross2b_byte2
    mov     r10d, 0x01
    test    eax, eax
    cmovnz  eax, r10d                           ; Return 1 if first byte is non-zero
    ret

.pgcross2b_byte2:
    mov     eax, 0x02                           ; Offset

    cmp     r9d, eax                            ; Check if aligned to page boundary
    je      .cpy128b

    movzx   ecx, byte [rsi + 0x02]              ; Copy third byte
    mov     byte [rdi + 0x02], cl

    mov     r9d, 0x03                           ; Incremented size
    test    ecx, ecx
    cmovnz  eax, r9d                            ; If not null, increment size and jump
    jnz     .cpy128b
    ret

.pgcross4b:
    vpxor   ymm15, ymm15

    vmovd   xmm0, [rsi]                         ; Load, compare and store first dword
    vpcmpeqb    xmm4, xmm0, xmm15
    vmovd   [rdi], xmm0

    vpmovmskb   ecx, xmm4                       ; Check for null in low 4 bytes
    tzcnt   eax, ecx
    cmp     eax, 0x04
    jnb     .pgcross4b_dword1
    vzeroupper
    ret

.pgcross4b_dword1:
    mov     eax, 0x04                           ; Offset

    cmp     r9d, eax                            ; Check if aligned to page boundary
    je      .cpy128b

    mov     ecx, r9d                            ; Set up offset for final qword
    sub     ecx, eax
    lea     eax, [eax + ecx - 0x04]

    vmovd   xmm1, [rsi + rax]                   ; Load, compare and store final dword
    vpcmpeqb    xmm5, xmm1, xmm15
    vmovd   [rdi + rax], xmm1

    vpmovmskb   ecx, xmm5                       ; Check for null in low 4 bytes
    tzcnt   r8d, ecx
    lea     eax, [eax + r8d]
    cmp     r8d, 0x04
    jnb     .cpy128b
    vzeroupper
    ret

.pgcross8b:
    vpxor   xmm15, xmm15, xmm15

    vmovq   xmm0, [rsi]                         ; Load, compare and store first qword
    vpcmpeqb    xmm4, xmm0, xmm15
    vmovq   [rdi], xmm0

    vpmovmskb   ecx, xmm4                       ; Check for null in low 8 bytes
    tzcnt   eax, ecx
    cmp     eax, 0x08
    jnb     .pgcross8b_qword1
    vzeroupper
    ret

.pgcross8b_qword1:
    mov     eax, 0x08                           ; Offset

    cmp     r9d, eax                            ; Check if aligned to page
    je      .cpy128b

    mov     ecx, r9d                            ; Set up offset for final qword
    sub     ecx, eax
    lea     eax, [eax + ecx - 0x08]

    vmovq   xmm1, [rsi + rax]                   ; Load, compare and store final qword
    vpcmpeqb    xmm5, xmm1, xmm15
    vmovq   [rdi + rax], xmm1

    vpmovmskb   ecx, xmm5
    tzcnt   r8d, ecx
    lea     eax, [eax + r8d]
    cmp     r8d, 0x08
    jnb     .cpy128b
    vzeroupper
    ret

.pgcross16b:
    vpxor   xmm15, xmm15, xmm15

    vmovdqu xmm0, [rsi]                         ; Load, compare and store first xmmword
    vpcmpeqb    xmm4, xmm0, xmm15
    vmovdqu [rdi], xmm0

    vptest  xmm4, xmm4                          ; Check for null
    jz      .pgcross16b_xmmwd1
    vpmovmskb   ecx, xmm4                       ; Return value
    tzcnt   eax, ecx
    vzeroupper
    ret

.pgcross16b_xmmwd1:
    mov     eax, 0x10                           ; Offset

    cmp     r9d, eax                            ; Check if aligned to page
    je      .cpy128b

    mov     ecx, r9d                            ; Set up offset for final xmmword
    sub     ecx, eax
    lea     eax, [eax + ecx - 0x10]

    vmovdqu xmm1, [rsi + rax]                   ; Load, compare and store final xmmword
    vpcmpeqb    xmm5, xmm1, xmm15
    vmovdqu [rdi + rax], xmm1

    lea     eax, [eax + 0x10]                   ; Advance offset

    vptest  xmm5, xmm5                          ; Check if null byte was encountered
    jz      .cpy128b

    vpmovmskb   ecx, xmm5                       ; Compute offset of null byte in xmmword
    tzcnt   r8d, ecx
    lea     eax, [eax + r8d - 0x10]             ; Return value
    vzeroupper
    ret

    jmp     .cpy128b                            ; Aligned to page boundary

.bufend_unaligned:
    section .data
    align   8
.bufendtbl:
    dq  .bufend1b
    dq  .bufend2b
    dq  .bufend4b
    dq  .bufend8b
    dq  .bufend16b

    section .text

    lea     r8, [.bufendtbl]                    ; Load jump table
    lzcnt   ecx, edx                            ; Number of leading zero bits
    neg     rcx                                 ; Two's complement
    jmp     [r8 + (rcx + 0x1f) * 8]             ; Jump to branch, rcx + 0x1f -> most significant set bit in edx, zero-indexed

.bufend1b:
    movzx   eax, byte [rsi]                     ; Load byte
    mov     byte [rdi], 0x00                    ; Null terminate
    mov     rcx, -E2BIG
    test    eax, eax
    cmovnz  rax, rcx                            ; Return -E2BIG if not null
    ret

.bufend2b:
    movzx   eax, byte [rsi]                     ; First byte
    mov     byte [rdi], al
    movzx   ecx, byte [rsi + 0x01]              ; Second byte
    mov     byte [rdi + 0x01], cl

    and     ecx, eax                            ; Check whether either byte was null
    jnz     .bufend2b_byte2
    mov     r10d, 0x01
    test    eax, eax
    cmovnz  eax, r10d                           ; Return 1 if first byte is non-zero
    ret

.bufend2b_byte2:
    cmp     edx, 0x02                           ; Check for end of buffer
    jna     .epi_ovf

    movzx   edx, byte [rsi + 0x02]              ; Final byte
    mov     eax, 0x02                           ; Return value if final byte is null
    mov     rcx, -E2BIG                         ; Return vlaue if final byte is non-null
    test    edx, edx                            ; Check if null
    cmovnz  rax, rcx                            ; Set -E2BIG if not null
    ret

.bufend4b:
    vpxor   ymm15, ymm15

    vmovd   xmm0, [rsi]                         ; Load, compare and store first dword
    vpcmpeqb    xmm4, xmm0, xmm15
    vmovd  [rdi], xmm0

    vpmovmskb   ecx, xmm4                       ; Check for null byte in low 4 lanes
    tzcnt   eax, ecx
    cmp     eax, 0x04
    jnb     .bufend4b_dword1
    vzeroupper
    ret

.bufend4b_dword1:
    cmp     edx, 0x04                           ; Check for end of buffer
    jna     .epi_ovf_clear

    mov     eax, 0x04                           ; Set up offset for final dword
    mov     ecx, edx
    sub     ecx, eax
    neg     ecx                                 ; Two's complement
    lea     eax, [ecx + 0x04]

    vmovd   xmm1, [rsi + rax]                   ; Load, compare and store final dword
    vpcmpeqb    xmm5, xmm1, xmm15
    vmovd   [rdi + rax], xmm1

    vpmovmskb   ecx, xmm5                       ; Check for null byte in low 4 lanes
    tzcnt   r8d, ecx
    cmp     r8d, 0x04
    jnb     .epi_ovf_clear
    lea     eax, [eax + r8d]
    vzeroupper
    ret

.bufend8b:
    vpxor   ymm15, ymm15

    vmovq   xmm0, [rsi]                         ; Load, compare and store first qword
    vpcmpeqb    xmm4, xmm0, xmm15
    vmovq   [rdi], xmm0

    vpmovmskb   ecx, xmm4                       ; Check for null byte in low 8 lanes
    tzcnt   eax, ecx
    cmp     eax, 0x08
    jnb     .bufend8b_qword1
    vzeroupper
    ret

.bufend8b_qword1:
    cmp     edx, 0x08                           ; Check for end of buffer
    jna     .epi_ovf_clear

    mov     eax, 0x08                           ; Set up offset for final qword
    mov     ecx, edx
    sub     ecx, eax
    neg     ecx                                 ; Two's complement
    lea     eax, [ecx + 0x08]

    vmovq   xmm1, [rsi + rax]                   ; Load, compare and store final qword
    vpcmpeqb    xmm5, xmm1, xmm15
    vmovq   [rdi + rax], xmm1

    vpmovmskb   ecx, xmm5                       ; Check for ull byte in low 8 lanes
    tzcnt   edx, ecx
    cmp     edx, 0x08
    jnb     .epi_ovf_clear
    lea     eax, [eax + edx]
    vzeroupper
    ret

.bufend16b:
    vpxor   ymm15, ymm15

    vmovdqu xmm0, [rsi]                         ; Load, compare and store first xmmword
    vpcmpeqb    xmm4, xmm0, xmm15
    vmovdqu [rdi], xmm0

    vptest  xmm4, xmm4                          ; Check if null byte was encountered
    jz      .bufend16b_xmmwd1

    vpmovmskb   ecx, xmm4                       ; Compute offset of null byte in xmmword
    tzcnt   eax, ecx
    vzeroupper
    ret

.bufend16b_xmmwd1:
    cmp     edx, 0x10                           ; Check for end of buffer
    jna     .epi_ovf_clear

    mov     eax, 0x10                           ; Set up offset for final xmmword
    mov     ecx, edx
    sub     ecx, eax
    neg     ecx                                 ; Two's complement
    lea     eax, [eax + 0x10]

    vmovdqu ymm1, [rsi + rax]                   ; Load, compare and store final xmmword
    vpcmpeqb    xmm5, xmm1, xmm15
    vmovdqu [rdi + rax], ymm1

    vptest  xmm5, xmm5
    jz      .epi_ovf_clear

    vpmovmskb   ecx, xmm5
    tzcnt   edx, ecx
    lea     eax, [eax + edx]
    vzeroupper
    ret

.epi_ovf_clear:
    vzeroupper
.epi_ovf:
    mov     rax, -E2BIG                         ; Return -E2BIG
    mov     byte [rdi + rdx - 0x01], 0x00       ; Null terminate
    ret

.epi_empty:
    mov     rax, -E2BIG
    ret
