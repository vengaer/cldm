    section .text

    default rel

    global cldm_avx2_scan_lt

    %macro alignjmp 0
        lea     r9, [rdi + rax]             ; Address of current byte
        tzcnt   r10, r9                     ; Index of least significant bit
        cmp     r10d, 0x4                   ; Clamp
        cmova   r10d, r8d
        shl     r10d, 3                     ; Multiply for offset
        jmp     [rcx + r10]
    %endmacro

    %macro chkbyte 4
        cmp     %1, %2                      ; Check for sentinel
        jb      .epilogue
        add     eax, 1                      ; Increment offset
        %if %4 != 0
            shr     %3, %4                  ; Shift out low byte
        %endif
    %endmacro

; Scan byte serquence and return index
; of first byte less than esi. Similar
; to strlen but with every value
; smaller than esi is treated like 0.
; Params:
;   rdi: Address of byte sequence
;   esi: Upper bound for byte to search
;        for, non-inclusive
; Return:
;   eax: Offset of first byte less than
;        esi
cldm_avx2_scan_lt:
    section .data
.align_table:
    dq .rdbyte
    dq .rdword
    dq .rddword
    dq .rdqword
    dq .rdxmmword
    dq .rdymmword

    section .text

    xor     eax, eax

    vpxor   xmm2, xmm2, xmm2                ; Shuffle mask
    vmovd   xmm1, esi                       ; Insert dword
    vpshufb xmm0, xmm1, xmm2                ; Broadcast byte to all lanes
    vpcmpeqb    ymm1, ymm3, ymm3            ; All ones

    mov     r8d, 0x5                        ; For clamping jump offset
    lea     rcx, [.align_table]             ; Load jump table
    alignjmp

.rdbyte:
    movzx   edx, byte [rdi]                 ; Load byte
    chkbyte edx, esi, edx, 0
    alignjmp

.rdword:
    movzx   edx, word [rdi + rax]           ; Load word
    chkbyte dl, sil, edx, 8
    chkbyte edx, esi, edx, 0
    alignjmp

.rddword:
    mov     edx, dword [rdi + rax]          ; Load dword
    chkbyte dl, sil, edx, 8
    chkbyte dl, sil, edx, 8
    chkbyte dl, sil, edx, 8
    chkbyte edx, esi, edx, 0
    alignjmp

.rdqword:
    mov     rdx, qword [rdi + rax]          ; Load qword
    chkbyte dl, sil, rdx, 8
    chkbyte dl, sil, rdx, 8
    chkbyte dl, sil, rdx, 8
    chkbyte dl, sil, rdx, 8
    chkbyte dl, sil, rdx, 8
    chkbyte dl, sil, rdx, 8
    chkbyte dl, sil, rdx, 8
    chkbyte edx, esi, rdx, 0
    alignjmp

.rdxmmword:
    vmovdqa xmm2, [rdi + rax]               ; Load xmmword
    vpcmpgtb    xmm3, xmm2, xmm0            ; Compare for greater
    vptest  xmm3, xmm1                      ; Set carry if nand yields zero
    jnc     .sntfound

    add     eax, 0x10                       ; Advance offset

.rdymmword:
    vmovdqa ymm2, [rdi + rax]               ; Load ymmword
    vpcmpgtb    ymm3, ymm2, ymm0            ; Compare for greater
    vptest  ymm3, ymm1                      ; Set carry if nand yields zero
    jnc     .sntfound

    add     eax, 0x20                       ; Advance offset
    jmp .rdymmword

.sntfound:
    vpmovmskb   rdx, ymm3                   ; Extract bitmask
    not     edx                             ; Invert
    tzcnt   ecx, edx                        ; Vector index of sentinel
    add     eax, ecx

.epilogue:
    vzeroupper
    ret
