#ifndef BUCKETS_INCLUDED
#define BUCKETS_INCLUDED

#include <immintrin.h>

#define OPTIMIZED_STRLEN

const size_t START_ELEMENTS_IN_BUCKET = 16;

/// @brief struct for one element in the table
typedef struct elem {
    union {
        char * long_name;
        __m128i short_name;
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
