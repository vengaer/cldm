    section .text

    default rel

    %define E2BIG 7

    %macro writebyte 1
        mov     byte [rdi + rax], r8b       ; Write

        %ifdef ZEROCHECK
            test    r8d, 0xff               ; Test for null
            jz      .epilogue
        %endif

        add     rax, 1                      ; Increment size
        %ifndef NO_OVFCHECK
            cmp     rax, rdx                ; Check for end of destination
            jnb     .epi_ovf
        %endif

        %if %1 != 0
            %ifdef SHR32
                shr     r8d, %1             ; Shift byte into position
            %else
                shr     r8, %1              ; Shift byte into position
            %endif
        %endif
    %endmacro

    %macro alignjmp 1
        %if %1 != 0
            add rsi, %1                     ; Advance source address
        %endif
        tzcnt   r9, rsi                     ; Index of least significant set bit
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
;     rdi: address of destination
;     rsi: address of source
;     rdx: size of destination
; Return:
;     rax: Size of the string written to
;          [rdx], or -7 if the length of
;          [rsi] is greater than rdx
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
    alignjmp  0                             ; Jump to branch

%define ZEROCHECK                           ; Enable null check for read byte
%define SHR32                               ; Shift only low 32 bits of r8 in writebyte

.rdbyte:                                    ; Source aligned to 1 byte boundary
    movzx   r8d, byte [rsi]                 ; First source byte

    writebyte 0                             ; Single byte
    alignjmp  1                             ; Jump to branch

.rdword:                                    ; Source aligned to 2 byte boundary
    movzx   r8d, word [rsi]                 ; Read word from source

    writebyte 8                             ; First byte
    writebyte 0                             ; Second byte

    alignjmp  2                             ; Jump to branch

.rddword:                                   ; Source aligned to 4 byte boundary
    mov     r8d, dword [rsi]                ; Read dword from source

    writebyte 8                             ; First byte
    writebyte 8                             ; Second byte
    writebyte 8                             ; Third byte
    writebyte 0                             ; Fourth byte

    alignjmp  4                             ; Jump to branch

%undef SHR32

.rdqword:                                   ; Source aligned to 8 byte boundary
    mov     r8, qword [rsi]                 ; Read qword

    writebyte 8                             ; First byte
    writebyte 8                             ; Second byte
    writebyte 8                             ; Third byte
    writebyte 8                             ; Fourth byte
    writebyte 8                             ; Fifth byte
    writebyte 8                             ; Sixth byte
    writebyte 8                             ; Seventh byte
    writebyte 0                             ; Eight byte

    alignjmp  8                             ; Jump to branch

%undef ZEROCHECK

.rdxmmword:                                 ; Source aligned to 16 byte boundary
    vmovdqa xmm0, [rsi]                     ; Read xmmword
    vpcmpeqb    xmm1, xmm0, xmm15           ; Compare bytes to null
    vpmovmskb   r8d, xmm1                   ; Extract byte mask

    test    r8d, r8d                        ; Set bit indicates null byte in loaded xmmword
    jnz     .xmmword_null

    mov     r9, rdx                         ; Remaining number of bytes in destination
    sub     r9, rax
    cmp     r9, 0x10                        ; Check room for 16 bytes
    jb      .xmmword_ovf

    vmovdqu [rdi + rax], xmm0               ; Write xmmword
    add     rax, 0x10                       ; Increment size
    add     rsi, 0x10                       ; Advance source address

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
    writebyte 8                             ; Write lsb

    sub     ecx, 1                          ; Decrement counter
    jnz     .xmmword_null_wrlow

    test    r9d, r9d                        ; Check if done
    jz      .epi_term

    vpsrldq xmm0, xmm0, 8                   ; Shift out low qword
    vmovq   r8, xmm0                        ; Low qword to r8

.xmmword_null_wrhigh:
    writebyte 8                             ; Write lsb

    sub     r9d, 1                          ; Decrement counter
    jnz     .xmmword_null_wrhigh

    jmp     .epi_term

.xmmword_ovf:                               ; Destination cannot hold entire xmmword, all bytes non-null

%define NO_OVFCHECK                         ; Already know how many bytes to write

    mov     ecx, r9d                        ; Number of remaining bytes
    mov     r10d, 8                         ; Size of qword
    cmp     ecx, r10d
    cmova   ecx, r10d                       ; Clamp to size of qword

    sub     r9d, ecx                        ; Subtract number of bytes to be checked

    vmovq   r8, xmm0

.xmmword_ovf_wrlow:
    writebyte 8                             ; Write lsb

    sub     ecx, 1                          ; Decrement counter
    jnz     .xmmword_ovf_wrlow

    test    r9d, r9d                        ; Check if done
    jz      .epi_ovf

    vpsrldq xmm0, xmm0, 8                   ; Shift out low qword
    vmovq   r8, xmm0                        ; Low qword to r8

.xmmword_ovf_wrhigh:
    writebyte 8                             ; Write lsb

    sub     r9d, 1                          ; Decrement counter
    jnz     .xmmword_ovf_wrhigh

    jmp     .epi_ovf

%undef NO_OVFCHECK

.rdymmword:                                 ; Source aligned to 32 byte boundary
    vmovdqa ymm0, [rsi]                     ; Read ymmword
    vpcmpeqb    ymm1, ymm0, ymm15           ; Compare bytes to null
    vpmovmskb   r8d, ymm1                   ; Extract byte mask

    test    r8d, r8d                        ; Set bit indicates null byte in loaded ymmword
    jnz     .ymmword_null

    mov     r9, rdx                         ; Remaining number of bytes in destination
    sub     r9, rax
    cmp     r9, 0x20                        ; Check room for 32 bytes
    jb      .ymmword_ovf

    vmovdqu [rdi + rax], ymm0               ; Write ymmword
    add     rax, 0x20                       ; Increment size
    add     rsi, 0x20                       ; Advance source address

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
    writebyte 8                             ; Write lsb

    sub     ecx, 1                          ; Decrement counter
    jnz     .ymmword_null_lxmmwd_wrlow

    test    r9d, r9d                        ; Check if done
    jz      .epi_term

    vpsrldq xmm0, xmm0, 8                   ; Shift out low qword
    vmovq   r8, xmm0                        ; Low qword to r8

.ymmword_null_lxmmwd_wrhigh:
    writebyte 8                             ; Write lsb

    sub     r9d, 1                          ; Decrement counter
    jnz     .ymmword_null_lxmmwd_wrhigh

    jmp     .epi_term

.ymmword_null_ovf:                          ; Null byte in high xmmword, destination cannot hold low xmmword
    mov     r9, rdx                         ; Remaining number of bytes in destination
    sub     r9, rax
    jmp     .ymmword_ovf_xmmwd

.ymmword_ovf:                               ; Destination cannot hold entire ymmword, all bytes non-null
    cmp     r9d, 0x10                       ; Compare number of remaining bytes against size of xmmword
    jb      .ymmword_ovf_xmmwd

    vmovdqu [rdi + rax], xmm0               ; Write low xmmword
    add     rax, 0x10                       ; Increment size
    sub     r9d, 0x10                       ; Subtract 16 from number of remaining bytes

    jz      .epi_ovf                        ; Done if no bytes remain

    vextracti128    xmm0, ymm0, 1           ; Replace low xmmword with high

.ymmword_ovf_xmmwd:                         ; Destination cannot hold low xmmword

%define NO_OVFCHECK                         ; Already know how many bytes to write

    test    r9d, r9d                        ; Check for end
    jz      .epi_ovf

    mov     ecx, r9d                        ; Number of remaining bytes
    mov     r10d, 8                         ; Size of qword
    cmp     ecx, r10d
    cmova   ecx, r10d                       ; Clamp to size of qword

    sub     r9d, ecx                        ; Subtract number of bytes to be checked

    vmovq   r8, xmm0                        ; Low qword to r8

.ymmword_ovf_xmmwd_wrlow:
    writebyte 8                             ; Write lsb

    sub     ecx, 1                          ; Decrement counter
    jnz     .ymmword_ovf_xmmwd_wrlow

    test    r9d, r9d                        ; Check if done
    jz      .epi_ovf

    vpsrldq xmm0, xmm0, 8                   ; Shift out low qword
    vmovq   r8, xmm0                        ; Low qword to r8

.ymmword_ovf_xmmwd_wrhigh:
    writebyte 8                             ; Write lsb

    sub     r9d, 1                          ; Decrement counter
    jnz     .ymmword_ovf_xmmwd_wrhigh

    jmp     .epi_ovf

%undef NO_OVFCHECK

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
    setnz   al                              ; Set if source is non-empty
    imul    rax, -E2BIG                     ; Return -E2BIG if length of source is > 0, otherwise 0
    ret
