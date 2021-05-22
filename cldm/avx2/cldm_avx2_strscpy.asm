    section .text

    default rel

    %define E2BIG 7

    %macro writebyte 4
        mov     byte [rdi + rax], r8b       ; Write

        %if %2 != 0
            test    r8d, 0xff               ; Test for null
            jz      .epilogue
        %endif

        add     rax, 1                      ; Increment size
        %if %3 != 0
            cmp     rax, rdx                ; Check for end of destination
            jnb     .epi_ovf
        %endif

        %if %1 != 0
            shr         %4, %1              ; Shift byte into position
        %endif
    %endmacro

    %macro alignjmp 0
        lea     r10, [rsi + rax]            ; Load address of current byte
        tzcnt   r9, r10                     ; Index of least significant set bit
        cmp     r9d, 0x4                    ; Check for 32 byte alignment
        cmova   r9d, r11d                   ; Clamp jump address to .rdymmword

        shl     r9d, 0x3                    ; Multiply by 8 for offset
        jmp     [rcx + r9]                  ; Jump to branch
    %endmacro

    global cldm_avx2_strscpy

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
; risk crossing page boundaries.
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

    section .data
.align_table:
    dq .rdbyte
    dq .rdword
    dq .rddword
    dq .rdqword
    dq .rdxmmword
    dq .rdymmword

    section .text

    xor     eax, eax                        ; Destination length

    cmp     rdx, 0                          ; Check for zero-size destination buffer
    jna     .dstsize0

    mov     r11d, 0x5                       ; Set r11 to 5 for clamping jump offset

    vpxor   ymm15, ymm15                    ; Zero for detecting null in x/ymmword

    lea     rcx, [.align_table]             ; Load jump table
    alignjmp                                ; Jump to branch

.rdbyte:                                    ; Source aligned to 1 byte boundary
    movzx   r8d, byte [rsi]                 ; First source byte

    writebyte 0, 1, 1, r8d                  ; Single byte
    alignjmp                                ; Jump to branch

.rdword:                                    ; Source aligned to 2 byte boundary
    movzx   r8d, word [rsi + rax]           ; Read word from source

    writebyte 8, 1, 1, r8d                  ; First byte
    writebyte 0, 1, 1, r8d                  ; Second byte

    alignjmp                                ; Jump to branch

.rddword:                                   ; Source aligned to 4 byte boundary
    mov     r8d, dword [rsi + rax]          ; Read dword from source

    writebyte 8, 1, 1, r8d                  ; First byte
    writebyte 8, 1, 1, r8d                  ; Second byte
    writebyte 8, 1, 1, r8d                  ; Third byte
    writebyte 0, 1, 1, r8d                  ; Fourth byte

    alignjmp                                ; Jump to branch

.rdqword:                                   ; Source aligned to 8 byte boundary
    mov     r8, qword [rsi + rax]           ; Read qword

    writebyte 8, 1, 1, r8                   ; First byte
    writebyte 8, 1, 1, r8                   ; Second byte
    writebyte 8, 1, 1, r8                   ; Third byte
    writebyte 8, 1, 1, r8                   ; Fourth byte
    writebyte 8, 1, 1, r8                   ; Fifth byte
    writebyte 8, 1, 1, r8                   ; Sixth byte
    writebyte 8, 1, 1, r8                   ; Seventh byte
    writebyte 0, 1, 1, r8                   ; Eighth byte

    alignjmp                                ; Jump to branch

.rdxmmword:                                 ; Source aligned to 16 byte boundary
    vmovdqa xmm0, [rsi + rax]               ; Read xmmword
    vpcmpeqb    xmm1, xmm0, xmm15           ; Compare bytes to null
    vpmovmskb   r8d, xmm1                   ; Extract byte mask

    test    r8d, r8d                        ; Set bit indicates null byte in loaded xmmword
    jnz     .xmmword_null

    lea     r10, [rax + 0x10]               ; Check room for 16 bytes
    cmp     rdx, r10
    jb      .xmmword_ovf

    vmovdqu [rdi + rax], xmm0               ; Write xmmword
    add     rax, 0x10                       ; Increment size

    cmp     rax, rdx                        ; Compare against remaining bytes in destination
    jnb     .epi_ovf

    jmp     .rdymmword

.xmmword_null:                              ; Null byte in xmmword
    tzcnt   ecx, r8d                        ; Index of null byte in xmmword
    jz      .epi_term                       ; Done if first byte is null

    mov     r9d, ecx                        ; Copy bit position
    mov     r10d, 8                         ; Size of qword
    cmp     ecx, r10d
    cmova   ecx, r10d                       ; Clamp to size of qword

    sub     r9d, ecx                        ; Subtract number of bits to be written

    vmovq   r8, xmm0                        ; Low qword to r8

.xmmword_null_wrlow:
    writebyte 8, 0, 1, r8                   ; Write lsb

    sub     ecx, 1                          ; Decrement counter
    jnz     .xmmword_null_wrlow

    test    r9d, r9d                        ; Check if done
    jz      .epi_term

    vpsrldq xmm0, xmm0, 8                   ; Shift out low qword
    vmovq   r8, xmm0                        ; Low qword to r8

.xmmword_null_wrhigh:
    writebyte 8, 0, 1, r8                   ; Write lsb

    sub     r9d, 1                          ; Decrement counter
    jnz     .xmmword_null_wrhigh

    jmp     .epi_term

.xmmword_ovf:                               ; Destination cannot hold entire xmmword, all bytes non-null
    mov     r9, rdx                         ; Compute number of remaining bytes in destination
    sub     r9, rax

    mov     ecx, r9d                        ; Number of remaining bytes
    mov     r10d, 8                         ; Size of qword
    cmp     ecx, r10d
    cmova   ecx, r10d                       ; Clamp to size of qword

    sub     r9d, ecx                        ; Subtract number of bytes to be checked

    vmovq   r8, xmm0

.xmmword_ovf_wrlow:
    writebyte 8, 0, 0, r8                   ; Write lsb

    sub     ecx, 1                          ; Decrement counter
    jnz     .xmmword_ovf_wrlow

    test    r9d, r9d                        ; Check if done
    jz      .epi_ovf

    vpsrldq xmm0, xmm0, 8                   ; Shift out low qword
    vmovq   r8, xmm0                        ; Low qword to r8

.xmmword_ovf_wrhigh:
    writebyte 8, 0, 0, r8                   ; Write lsb

    sub     r9d, 1                          ; Decrement counter
    jnz     .xmmword_ovf_wrhigh

    jmp     .epi_ovf

.rdymmword:                                 ; Source aligned to 32 byte boundary
    vmovdqa ymm0, [rsi + rax]               ; Read ymmword
    vpcmpeqb    ymm1, ymm0, ymm15           ; Compare bytes to null
    vpmovmskb   r8d, ymm1                   ; Extract byte mask

    test    r8d, r8d                        ; Set bit indicates null byte in loaded ymmword
    jnz     .ymmword_null

    lea     r10, [rax + 0x20]               ; Check room for 32 bytes
    cmp     rdx, r10
    jb      .ymmword_ovf

    vmovdqu [rdi + rax], ymm0               ; Write ymmword
    add     rax, 0x20                       ; Increment size

    jmp     .rdymmword

.ymmword_null:                              ; Null byte in ymmword
    tzcnt   ecx, r8d                        ; Index of null byte
    jz      .epi_term                       ; Done is null in first byte

    cmp     ecx, 0x10                       ; Check against size of xmmword
    jb      .ymmword_null_lxmmwd

    cmp     edx, 0x10                       ; Compare against remaining bytes in destination
    jb      .ymmword_null_ovf

    vmovdqu [rdi + rax], xmm0               ; Write low xmmword
    add     rax, 0x10                       ; Increment counter
    sub     ecx, 0x10                       ; Subtract number of remaining bytes

    jz      .epi_term                       ; Done if no bytes remain

    vextracti128    xmm0, ymm0, 1           ; Replace low xmmword with high

.ymmword_null_lxmmwd:                       ; Null byte in low xmmword
    mov     r9d, ecx                        ; Copy bit position
    mov     r10d, 8                         ; Size of qword
    cmp     ecx, r10d
    cmova   ecx, r10d                       ; Clamp to size of qword

    sub     r9d, ecx                        ; Subtract number of bits to be written

    vmovq   r8, xmm0                        ; Low qword to r8

.ymmword_null_lxmmwd_wrlow:
    writebyte 8, 0, 1, r8                   ; Write lsb

    sub     ecx, 1                          ; Decrement counter
    jnz     .ymmword_null_lxmmwd_wrlow

    test    r9d, r9d                        ; Check if done
    jz      .epi_term

    vpsrldq xmm0, xmm0, 8                   ; Shift out low qword
    vmovq   r8, xmm0                        ; Low qword to r8

.ymmword_null_lxmmwd_wrhigh:
    writebyte 8, 0, 1, r8                   ; Write lsb

    sub     r9d, 1                          ; Decrement counter
    jnz     .ymmword_null_lxmmwd_wrhigh

    jmp     .epi_term

.ymmword_null_ovf:                          ; Null byte in high xmmword, destination cannot hold low xmmword
    mov     r9, rdx                         ; Remaining number of bytes in destination
    sub     r9, rax
    jmp     .ymmword_ovf_xmmwd

.ymmword_ovf:                               ; Destination cannot hold entire ymmword, all bytes non-null
    lea     r10, [rax + 0x10]               ; Check room for 16 bytes
    cmp     r10, rdx
    jnb      .ymmword_ovf_xmmwd

    vmovdqu [rdi + rax], xmm0               ; Write low xmmword
    add     rax, 0x10                       ; Increment size
    cmp     rax, rdx
    jnb     .epi_ovf

    vextracti128    xmm0, ymm0, 1           ; Replace low xmmword with high

.ymmword_ovf_xmmwd:                         ; Destination cannot hold low xmmword
    cmp     rax, rdx
    jnb     .epi_ovf

    mov     r9, rdx                         ; Compute number of remaining bytes in destination
    sub     r9, rax

    mov     ecx, r9d                        ; Number of remaining bytes
    mov     r10d, 8                         ; Size of qword
    cmp     ecx, r10d
    cmova   ecx, r10d                       ; Clamp to size of qword

    sub     r9d, ecx                        ; Subtract number of bytes to be checked

    vmovq   r8, xmm0                        ; Low qword to r8

.ymmword_ovf_xmmwd_wrlow:
    writebyte 8, 0, 0, r8                   ; Write lsb

    sub     ecx, 1                          ; Decrement counter
    jnz     .ymmword_ovf_xmmwd_wrlow

    test    r9d, r9d                        ; Check if done
    jz      .epi_ovf

    vpsrldq xmm0, xmm0, 8                   ; Shift out low qword
    vmovq   r8, xmm0                        ; Low qword to r8

.ymmword_ovf_xmmwd_wrhigh:
    writebyte 8, 0, 0, r8                   ; Write lsb

    sub     r9d, 1                          ; Decrement counter
    jnz     .ymmword_ovf_xmmwd_wrhigh

    jmp     .epi_ovf


.epi_term:
    mov     byte [rdi + rax], 0             ; Null terminate
.epilogue:
    vzeroupper                              ; Zero ymm state
    ret

.epi_ovf:                                   ; Source overflows destination
    mov     byte [rdi + rdx - 1], 0         ; Null terminate
    mov     rax, -E2BIG                     ; Return -E2BIG
    vzeroupper                              ; Zero ymm state
    ret

.dstsize0:                                  ; Destination size is 0
    test    byte [rsi], 0xff                ; Check for null
    mov     rdx, -E2BIG
    cmovnz  rax, rdx                        ; Return -E2BIG if length of source is > 0, otherwise 0
    ret
