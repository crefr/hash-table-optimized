#ifndef BUCKETS_INCLUDED
#define BUCKETS_INCLUDED

const size_t NAME_MAX_LEN = 64;

/// @brief struct for one element in the table
typedef struct elem {
    alignas(16) char name[NAME_MAX_LEN];

    void * data;
    size_t data_size;

    struct elem * next;
} elem_t;


/// @brief list header
typedef struct {
    elem_t * first_elem;
    size_t bucket_size;
} bucket_t;


void bucketInit(bucket_t * bucket);

void bucketDtor(bucket_t * bucket);

void * bucketLookup(bucket_t * bucket, const char * name);

void bucketInsert(bucket_t * bucket, const char * name, void * data, size_t data_size);

#endif
