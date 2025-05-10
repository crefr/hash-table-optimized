#ifndef STRLEN_STRCMP_OPT_INCLUDED
#define STRLEN_STRCMP_OPT_INCLUDED

#include <immintrin.h>

//! ONLY FOR STRINGS <= 16 chars and memory must be for 16 chars at str1 and str2
//! ONLY CHECKS FOR EQUALITY
//! str1 and str2 MUST BE ALIGNED TO 16
static inline int strcmp_optimized(__m128i str1, __m128i str2)
{
    __m128i cmp_result = _mm_cmpeq_epi8(str1, str2);

    uint32_t answer = (uint32_t)_mm_movemask_epi8(cmp_result);

    // we need to negate younger 16 bits
    answer ^= 0x0000FFFF;

    return answer;
}


//! str must be aligned as __m128i and be made of blocks of 16 bytes
static inline size_t strlen_optimized(const char * str)
{
    const char * cur_char = str;

    asm (
        "pxor xmm0, xmm0 \n\t"  // xmm0 is all zeros
        : : : "xmm0"
    );

    uint32_t ending = 0;

    while (! ending){
        asm (
            "movdqa xmm1, [%[cur_char]] \n\t"
            "pcmpeqb xmm1, xmm0         \n\t"
            "pmovmskb %[ending], xmm1   \n\t" // ending`s bit i is 1 if cur_block[i] = 0

            : [ending]   "=r" (ending)
            : [cur_char] "r" (cur_char)
            : "xmm1"
        );

        cur_char += 16;
    }

    uint32_t shift = 0;

    asm (
        "bsf %[shift], %[ending] \n\t"
        : [shift]  "=r" (shift)
        : [ending] "r"  (ending)
    );
    size_t len = (cur_char - str) - 16 + shift;

    return len;
}

#endif
