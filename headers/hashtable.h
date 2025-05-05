#ifndef HASHTABLE_INCUDED
#define HASHTABLE_INCUDED

#include "buckets.h"
// #include "hash.h"

/// @brief table struct
typedef struct {
    bucket_t * buckets;
    size_t table_size;
} table_t;

// #define calcHash MurMur32Hash
// #define calcHash crc32Hash
#define calcHash crc32_optimized

const size_t NAME_ALIGNMENT = 16;

table_t tableCtor(size_t table_size);

void tableDtor(table_t * table);

void * tableLookup(table_t * table, const char * name);

void tableInsert(table_t * table, const char * name, void * data, size_t data_size);

double tableTestDistribution(table_t * table);

#endif
