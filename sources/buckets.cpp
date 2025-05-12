#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <immintrin.h>
#include <stdint.h>

#include "strlen_strcmp_opt.h"
#include "buckets.h"
#include "hash.h"

static void newElem(bucket_t * bucket, const char * name, void * data, size_t data_size);

static void delElem(elem_t * elem);

static void bucketPush(bucket_t * bucket, const char * name, void * data, size_t data_size);

static void bucketRealloc(bucket_t * bucket);


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

    if (name_len < sizeof(__m128i)){
        alignas(sizeof(__m128i)) char aligned_name[sizeof(__m128i)] = "";
        strncpy(aligned_name, name, sizeof(__m128i) - 1);

        new_elem->short_name = _mm_load_si128((__m128i *)aligned_name);
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

    if (elem->name_len >= sizeof(__m128i))
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


//!!! name is aligned to sizeof(__m128i) !!!
void * bucketLookup(bucket_t * bucket, const char * name)
{
    assert(bucket);
    assert(name);

# ifdef OPTIMIZED_STRLEN
    size_t name_len = strlen_optimized(name);
# else
    size_t name_len = strlen(name);
#endif

    size_t bucket_size = bucket->bucket_size;

    if (name_len < sizeof(__m128i)){
        __m128i aligned_name = _mm_load_si128((__m128i *)name);

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


int bucketVerify(bucket_t * bucket, size_t bucket_index, size_t table_size)
{
    assert(bucket);

    size_t bucket_size = bucket->bucket_size;
    elem_t * elements  = bucket->elements;

    if (bucket_size >= bucket->elem_capacity){
        fprintf(stderr, "BUCKET ERROR: size >= capacity!\n");

        return 1;
    }

    for (size_t elem_index = 0; elem_index < bucket_size; elem_index++){
        const char * name = (elements[elem_index].name_len < sizeof(__m128i)) ?
            (char *)&(elements[elem_index].short_name):
            elements[elem_index].long_name;

        size_t name_len = strlen(name);

        uint32_t good_index = calcHash(name, name_len) % table_size;

        if (good_index != bucket_index){
            fprintf(stderr, "BUCKET ERROR: %s - incorrect bucket!\n"
                            "\tmust be in bucket %zu but in %zu\n", bucket_index, good_index);

            return 1;
        }
    }

    // no errors
    return 0;
}
