    section .text

    global cldm_tas8
    global cldm_tas16
    global cldm_tas32
    global cldm_tas64

    %macro  tas 3
        mov     %1, %2
    .try_xchg:
        lock cmpxchg    %2, %3
        jnz     .try_xchg
    %endmacro

; 8 bit atomic test and set
; Params:
;     rdi: address to write
;     sil: value to set
; Return:
;     -
cldm_tas8:
    tas     al, byte [rdi], sil
    ret

; 16 bit atomic test and set
; Params:
;     rdi: address to write
;     si:  value to set
; Return:
;     -
cldm_tas16:
    tas     ax, word [rdi], si
    ret

; 32 bit atomic test and set
; Params:
;     rdi: address to write
;     esi: value to set
; Return:
;     -
cldm_tas32:
    tas     eax, dword [rdi], esi
    ret

; 64 bit atomic test and set
; Params:
;     rdi: address to write
;     rsi: value to set
; Return:
;     -
cldm_tas64:
    tas     rax, qword [rdi], rsi
    ret
