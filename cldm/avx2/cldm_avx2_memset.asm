    section .text

    default rel

    %macro resjmp 1
        %if %1 != 0
            cmp     rdi, r10                ; Check if done
            jnb     .residual_done
        %endif

        tzcnt   rcx, rdi                    ; Index of least significant set bit
        cmp     ecx, 0x2                    ; Check for 8 byte alignment
        cmova   ecx, r8d                    ; Clamp jump offset

        shl     ecx, 0x3                    ; Multiply by 8 for offset
        jmp     [r9 + rcx]
    %endmacro

    global cldm_avx2_memset

; AVX2-accelerated memset
; Params:
;   rdi: Destination address
;   esi: Value to write to each
;        destination byte
;   rdx: Number of bytes to write
; Return:
;   rax: Destination address
cldm_avx2_memset:
    mov     rax, rdi                        ; Return destination
    cmp     rdx, 0x20                       ; Check space for 32 bytes
    jb      .dlt32b
    je      .deq32b

    xor     ecx, ecx                        ; Address offset

    vpxor   xmm2, xmm2, xmm2                ; Shuffle mask
    vmovd   xmm1, esi                       ; Insert dword
    vpshufb     xmm0, xmm1, xmm2            ; Broadcast 0th byte
    vinserti128 ymm1, ymm0, xmm0, 0x1       ; Copy low xmmword to high

    tzcnt   r8, rdi                         ; Least significant set bit
    cmp     r8d, 0x5                        ; Check if aligned
    jnb     .wr256

    vmovdqu [rdi], ymm1                     ; Write ymmword
    add     rdi, 0x1f
    and     rdi, -0x20                      ; Align

    lea     rdx, [rdx + rax]                ; Compute number of remaining bytes
    sub     rdx, rdi

.wr256:
    add     rcx, 0x100                      ; Advance offset
    cmp     rdx, rcx                        ; Check room for 256 bytes
    jb      .wrlt256b                       ; Less than 256 bytes remaining
    vmovdqa [rdi + rcx - 0x0100], ymm1      ; Write 256 bytes
    vmovdqa [rdi + rcx - 0x00e0], ymm1
    vmovdqa [rdi + rcx - 0x00c0], ymm1
    vmovdqa [rdi + rcx - 0x00a0], ymm1
    vmovdqa [rdi + rcx - 0x0080], ymm1
    vmovdqa [rdi + rcx - 0x0060], ymm1
    vmovdqa [rdi + rcx - 0x0040], ymm1
    vmovdqa [rdi + rcx - 0x0020], ymm1
    ja      .wr256                          ; Jump if there are bytes remaining

    vzeroupper
    ret

.wrlt256b:
    sub     ecx, 0x80
    cmp     edx, ecx                        ; Check room for 128 bytes
    jb      .wrlt128b                       ; Less than 128 bytes remaining
    vmovdqa [rdi + rcx - 0x80], ymm1        ; Write 128 bytes
    vmovdqa [rdi + rcx - 0x60], ymm1
    vmovdqa [rdi + rcx - 0x40], ymm1
    vmovdqa [rdi + rcx - 0x20], ymm1
    je      .wrepi
    add     ecx, 0x80                       ; Advance offset

.wrlt128b:
    sub     ecx, 0x40
    cmp     edx, ecx                        ; Check room for 64 bytes
    jb      .wrlt64b                        ; Less than 64 bytes remaining
    vmovdqa [rdi + rcx - 0x40], ymm1        ; Write 64 bytes
    vmovdqa [rdi + rcx - 0x20], ymm1
    je      .wrepi
    add     ecx, 0x40

.wrlt64b:
    sub     ecx, 0x20
    cmp     edx, ecx                        ; Check room for 32 bytes
    jb      .wrlt32b
    vmovdqa [rdi + rcx - 0x20], ymm1        ; Write ymmword
    je      .wrepi
    add     ecx, 0x20

.wrlt32b:
    sub     ecx, 0x10
    cmp     edx, ecx                        ; Check room for 16 bytes
    jb      .wrlt16b
    vmovdqa [rdi + rcx - 0x10], xmm1        ; Write xmmword
    je      .wrepi
    add     ecx, 0x10

.wrlt16b:
    mov     r11, 0x0101010101010101
    imul    rsi, r11                        ; Broadcast low byte

    sub     ecx, 0x8                        ; Check room for qword
    cmp     edx, ecx
    jb      .wrlt8b
    mov     qword [rdi + rcx - 0x8], rsi    ; Write qword
    je      .wrepi
    add     ecx, 0x8

.wrlt8b:
    sub     ecx, 0x4                        ; Check room for dword
    cmp     edx, ecx
    jb      .wrlt4b
    mov     dword [rdi + rcx - 0x4], esi    ; Write dword
    je      .wrepi
    add     ecx, 0x4

.wrlt4b:
    sub     ecx, 0x2                        ; Check room for word
    cmp     edx, ecx
    jb      .wrlt2b
    mov     word [rdi + rcx - 0x2], si      ; Write word
    je      .wrepi
    add     ecx, 0x2

.wrlt2b:
    mov     byte [rdi + rcx - 0x2], sil

.wrepi:
    vzeroupper
    ret

.deq32b:
    vpxor   xmm2, xmm2, xmm2                ; Shuffle mask
    vmovd   xmm1, esi                       ; Insert dword
    vpshufb     xmm0, xmm1, xmm2            ; Broadcast 0th byte
    vinserti128 ymm1, ymm0, xmm0, 0x1       ; Copy low xmmword to high
    vmovdqu [rdi], ymm1                     ; Write ymmword
    vzeroupper
    ret

.dlt32b:
    cmp     rdx, 0x10                       ; Check space for 16 bytes
    jb      .dresidual

.deq16b:
    vpxor   xmm2, xmm2, xmm2                ; Shuffle mask
    vmovd   xmm1, esi                       ; Insert dword
    vpshufb     xmm0, xmm1, xmm2            ; Broadcast low byte
    vmovdqu [rdi], xmm0                     ; Write xmmword
    ja      .da16b
    vzeroupper
    ret

.da16b:
    lea     rdi, [rdi + 0x10]               ; Advance destination address

.dresidual:

    section .data
.residual_table:
    dq .wrbyte
    dq .wrword
    dq .wrdword
    dq .wrqword

    section .text

    lea     r9, [.residual_table]           ; Load jump table
    mov     r8d, 0x3                        ; For clamping jump offset
    lea     r10, [rax + rdx]                ; Upper bound for writes
    mov     r11, 0x0101010101010101
    imul    rsi, r11                        ; Broadcast low byte
    resjmp  0

.wrbyte:
    mov     byte [rdi], sil
    add     rdi, 1
    resjmp  1
.wrword:
    mov     word [rdi], si
    add     rdi, 2
    resjmp  1
.wrdword:
    mov     dword [rdi], esi
    add     rdi, 4
    resjmp  1
.wrqword:
    mov     qword [rdi], rsi
    add     rdi, 8
    resjmp  1

.residual_done:
    ret
