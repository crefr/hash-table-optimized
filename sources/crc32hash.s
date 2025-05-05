; uint32_t crc32_optimized(const void * data, const size_t len)
global crc32_optimized

section .text

;================================================
;--------------------------------------
; Calculate crc32 hash.
; Optimized by x86_64 crc32 instruction.
; Entry:
;   rdi = data ptr
;   rsi = data len
; Exit:
;   eax = hash
;--------------------------------------
crc32_optimized:
        ; push rbp
        ; mov rbp, rsp

        xor eax, eax            ; hash = 0
        mov rcx, rsi

    .hash_loop:
        crc32 eax, BYTE [rdi]
        inc rdi

        loop .hash_loop

        ; pop rbp
ret
;================================================

