#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <immintrin.h>
#include <stdint.h>

#include "buckets.h"

static void newElem(bucket_t * bucket, const char * name, void * data, size_t data_size);

static void delElem(elem_t * elem);

static void bucketPush(bucket_t * bucket, const char * name, void * data, size_t data_size);

static void bucketRealloc(bucket_t * bucket);

//! ONLY FOR STRINGS <= 16 chars and memory must be for 16 chars at str1 and str2
//! ONLY CHECKS FOR EQUALITY
//! str1 and str2 MUST BE ALIGNED TO 16
static inline int strcmp_optimized(mXXXi str1, mXXXi str2)
{
    mXXXi cmp_result = mm_cmpeq_epi8(str1, str2);

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


static void newElem(bucket_t * bucket, const char * name, void * data, size_t data_size)
{
    assert(name);
    assert(data);

    if (bucket->bucket_size == bucket->elem_capacity - 1)
        bucketRealloc(bucket);

    elem_t * new_elem = &(bucket->elements[bucket->bucket_size]);
    bucket->bucket_size++;

    size_t name_len = strlen(name);
    new_elem->name_len = name_len;

    if (name_len < sizeof(mXXXi)){
        alignas(sizeof(mXXXi)) char aligned_name[sizeof(mXXXi)] = "";
        strncpy(aligned_name, name, sizeof(mXXXi) - 1);

        new_elem->short_name = mm_load_siXXX((__m128i *)aligned_name);
    }
    else {
        new_elem->long_name = (char *)calloc(name_len + 1, sizeof(char));
        strncpy(new_elem->long_name, name, name_len);
    }

    new_elem->data = calloc(1, data_size);
    memcpy(new_elem->data, data, data_size);
}


static void delElem(elem_t * elem)
{
    assert(elem);

    if (elem->name_len >= sizeof(mXXXi))
        free(elem->long_name);

    free(elem->data);
}

void bucketInit(bucket_t * bucket)
{
    assert(bucket);

    bucket->bucket_size = 0;

    bucket->elem_capacity = START_ELEMENTS_IN_BUCKET;
    bucket->elements = (elem_t *)calloc(START_ELEMENTS_IN_BUCKET, sizeof(*(bucket->elements)));
}

void bucketDtor(bucket_t * bucket)
{
    assert(bucket);

    for (size_t elem_index = 0; elem_index < bucket->bucket_size; elem_index++)
        delElem(&bucket->elements[elem_index]);

    free(bucket->elements);
}

static void bucketRealloc(bucket_t * bucket)
{
    assert(bucket);

    bucket->elem_capacity *= 2;
    bucket->elements = (elem_t *)realloc(bucket->elements, sizeof(*(bucket->elements)) * bucket->elem_capacity);
}


//!!! name is aligned to sizeof(mXXXi) !!!
void * bucketLookup(bucket_t * bucket, const char * name)
{
    assert(bucket);
    assert(name);

    size_t name_len = strlen(name);
    size_t bucket_size = bucket->bucket_size;

    if (name_len < sizeof(mXXXi)){
        // mXXXi aligned_name = mm_set1_epi32(0);
        // memcpy((char *)&aligned_name, name, name_len);

        mXXXi aligned_name = mm_load_siXXX((mXXXi *)name);

        elem_t * cur_elem = bucket->elements;

        // bucket->elements is just an array
        for (size_t elem_index = 0; elem_index < bucket_size; elem_index++){
            if (strcmp_optimized(cur_elem->short_name, aligned_name) == 0)
                return cur_elem->data;

            cur_elem++;
        }
    }
    else {
        // pretty rare scenario
        elem_t * cur_elem = bucket->elements;

        for (size_t elem_index = 0; elem_index < bucket_size; elem_index++){
            if (name_len != cur_elem->name_len){
                cur_elem++;
                continue;
            }

            if (strcmp(cur_elem->long_name, name) == 0)
                return cur_elem->data;
        }
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

    newElem(bucket, name, data, data_size);
}
