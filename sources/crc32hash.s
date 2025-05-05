; uint32_t crc32_optimized(const void * data, const size_t len)
global crc32_optimized

; uint32_t crc32_optimized_8byte(const void * data, const size_t len)
global crc32_optimized_8byte

section .text

;================================================
;--------------------------------------
; Calculate crc32 hash.
; Optimized by x86_64 crc32 instruction (1-byte variation)
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
        dec eax

        mov rcx, rsi

    .hash_loop:
        crc32 eax, BYTE [rdi]
        inc rdi

        loop .hash_loop

        xor eax, 0xFFFFFFFF

        ; pop rbp
ret
;================================================


;================================================
;--------------------------------------
; Calculate crc32 hash.
; Optimized by x86_64 crc32 instruction (8-byte variation)
; data must be aligned to 8 and must be of 8-byte blocks
; Entry:
;   rdi = data ptr MUST BE ALIGNED TO 8
;   rsi = data len
; Exit:
;   eax = hash
;--------------------------------------
crc32_optimized_8byte:
        ; push rbp
        ; mov rbp, rsp

        add rsi, 7
        shr rsi, 3              ; divide by 8 rounding upward

        mov eax, 0xFFFFFFFF

        mov rcx, rsi

    .hash_loop:
        crc32 rax, QWORD [rdi]
        add rdi, 8

        loop .hash_loop

        xor eax, 0xFFFFFFFF

        ; pop rbp
ret
;================================================

