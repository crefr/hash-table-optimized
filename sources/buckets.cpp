#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <immintrin.h>
#include <stdint.h>

#include "buckets.h"

static elem_t * newElem(const char * name, void * data, size_t data_size, elem_t * next);

static void delElem(elem_t * elem);

static void bucketPush(bucket_t * bucket, const char * name, void * data, size_t data_size);

#ifdef USING_AVX
    typedef __m256i mXXXi;

    #define mm_load_siXXX       _mm256_load_si256
    #define mm_cmpeq_epi8       _mm256_cmpeq_epi8
    #define mm_movemask_epi8    _mm256_movemask_epi8
#else
    typedef __m128i mXXXi;

    #define mm_load_siXXX       _mm_load_si128
    #define mm_cmpeq_epi8       _mm_cmpeq_epi8
    #define mm_movemask_epi8    _mm_movemask_epi8
#endif


//! ONLY FOR STRINGS <= 16 chars and memory must be for 16 chars at str1 and str2
//! ONLY CHECKS FOR EQUALITY
//! str1 and str2 MUST BE ALIGNED TO 16
static inline int strcmp_optimized(const char * str1, const char * str2)
{
    // printf("strcmp_optimized is used!!!\n");

    assert(str1);
    assert(str2);

    mXXXi str1_xmm = mm_load_siXXX ((mXXXi const*) str1);
    mXXXi str2_xmm = mm_load_siXXX ((mXXXi const*) str2);

    mXXXi cmp_result = mm_cmpeq_epi8(str1_xmm, str2_xmm);

    uint32_t answer = (uint32_t)mm_movemask_epi8(cmp_result);

# ifdef USING_AVX
    // we need to negate it
    answer = ~answer;
# else
    // we need to negate younger 16 bits
    answer ^= 0x0000FFFF;
# endif

    return answer;
}


static elem_t * newElem(const char * name, void * data, size_t data_size, elem_t * next)
{
    assert(name);
    assert(data);

    elem_t * new_elem = (elem_t *)calloc(1, sizeof(*new_elem));

    strncpy(new_elem->name, name, NAME_MAX_LEN);
    new_elem->name_len = strlen(name);

    new_elem->data = calloc(1, data_size);
    new_elem->data_size = data_size;
    new_elem->next = next;

    return new_elem;
}

static void delElem(elem_t * elem)
{
    assert(elem);

    free(elem->data);
    free(elem);
}

void bucketInit(bucket_t * bucket)
{
    assert(bucket);

    bucket->bucket_size = 0;
    bucket->first_elem = NULL;
}

void bucketDtor(bucket_t * bucket)
{
    assert(bucket);

    elem_t * cur_elem = bucket->first_elem;

    while(cur_elem != NULL){
        elem_t * old_elem = cur_elem;
        cur_elem = old_elem->next;

        delElem(old_elem);
    }
}

void * bucketLookup(bucket_t * bucket, const char * name)
{
    assert(bucket);
    assert(name);

    elem_t * cur_elem = bucket->first_elem;

    size_t name_len = strlen(name);

    // we are copying string to aligned in case we are using strcmp_optimized
    alignas(sizeof(mXXXi)) char aligned_name[sizeof(mXXXi)] = "";
    strncpy(aligned_name, name, sizeof(mXXXi) - 1);

    while (cur_elem != NULL){
        if (name_len != cur_elem->name_len){
            cur_elem = cur_elem->next;
            continue;
        }

        int strcmp_result = (name_len < sizeof(mXXXi)) ?
            strcmp_optimized(cur_elem->name, aligned_name) :
            strcmp(cur_elem->name, name);

        if (strcmp_result == 0)
            return cur_elem->data;

        cur_elem = cur_elem->next;
    }

    return NULL;
}

void bucketInsert(bucket_t * bucket, const char * name, void * data, size_t data_size)
{
    assert(bucket);
    assert(name);
    assert(data);

    if (bucketLookup(bucket, name) != NULL)
        return;

    bucketPush(bucket, name, data, data_size);
}

static void bucketPush(bucket_t * bucket, const char * name, void * data, size_t data_size)
{
    assert(bucket);
    assert(name);
    assert(data);

    elem_t * new_elem = newElem(name, data, data_size, bucket->first_elem);
    bucket->first_elem = new_elem;
    bucket->bucket_size++;

    memcpy(new_elem->data, data, data_size);
}
