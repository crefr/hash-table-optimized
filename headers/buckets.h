#ifndef BUCKETS_INCLUDED
#define BUCKETS_INCLUDED

#include <immintrin.h>

#define OPTIMIZED_STRLEN

// decide if using AVX instead of SSE
// #define USING_AVX

#ifdef USING_AVX
    typedef __m256i mXXXi;

    #define mm_set1_epi32       _mm256_set1_epi32
    #define mm_load_siXXX       _mm256_load_si256
    #define mm_cmpeq_epi8       _mm256_cmpeq_epi8
    #define mm_movemask_epi8    _mm256_movemask_epi8
#else
    typedef __m128i mXXXi;

    #define mm_set1_epi32       _mm_set1_epi32
    #define mm_load_siXXX       _mm_load_si128
    #define mm_cmpeq_epi8       _mm_cmpeq_epi8
    #define mm_movemask_epi8    _mm_movemask_epi8
#endif

const size_t START_ELEMENTS_IN_BUCKET = 16;

/// @brief struct for one element in the table
typedef struct elem {
    union {
        char * long_name;
        mXXXi short_name;
    };
    size_t name_len;

    void * data;
} elem_t;


/// @brief list header
typedef struct {
    elem_t * elements;

    size_t elem_capacity;
    size_t bucket_size;
} bucket_t;


void bucketInit(bucket_t * bucket);

void bucketDtor(bucket_t * bucket);

void * bucketLookup(bucket_t * bucket, const char * name);

void bucketInsert(bucket_t * bucket, const char * name, void * data, size_t data_size);

#endif
