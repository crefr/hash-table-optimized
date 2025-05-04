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

    new_elem->next = bucket->first_elem;
    bucket->first_elem = new_elem;

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

    new_elem->data_size = data_size;

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

    bucket->first_elem = NULL;
}

void bucketDtor(bucket_t * bucket)
{
    assert(bucket);

    elem_t * cur_elem = bucket->first_elem;

    while(cur_elem != NULL){
        elem_t * next_elem = cur_elem->next;
        delElem(cur_elem);
        cur_elem = next_elem;
    }
    free(bucket->elements);
}

static void bucketRealloc(bucket_t * bucket)
{
    assert(bucket);

    elem_t * old_elem_start = bucket->elements;

    bucket->elem_capacity *= 2;
    bucket->elements = (elem_t *)realloc(bucket->elements, sizeof(*(bucket->elements)) * bucket->elem_capacity);

    // recalculating pointers
    bucket->first_elem = bucket->elements + (bucket->first_elem - old_elem_start);
    elem_t * cur_elem = bucket->first_elem;
    while (cur_elem->next != NULL){
        cur_elem->next = bucket->elements + (cur_elem->next - old_elem_start);
        cur_elem = cur_elem->next;
    }
}

void * bucketLookup(bucket_t * bucket, const char * name)
{
    assert(bucket);
    assert(name);

    size_t name_len = strlen(name);

    mXXXi aligned_name = mm_set1_epi32(0);
    strncpy((char *)&aligned_name, name, sizeof(mXXXi) - 1);

    elem_t * next_elem = bucket->first_elem;
    while (next_elem != NULL){
        elem_t * cur_elem = next_elem;
        next_elem = cur_elem->next;

        if (name_len != cur_elem->name_len){
            cur_elem = cur_elem->next;
            continue;
        }

        int strcmp_result = (name_len < sizeof(mXXXi)) ?
            strcmp_optimized(cur_elem->short_name, aligned_name) :
            strcmp(cur_elem->long_name, name);

        if (strcmp_result == 0)
            return cur_elem->data;

        // cur_elem = cur_elem->next;
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
