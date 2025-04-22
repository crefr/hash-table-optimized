#ifndef HASHTABLE_INCUDED
#define HASHTABLE_INCUDED

#include "buckets.h"

/// @brief table struct
typedef struct {
    bucket_t * buckets;
    size_t table_size;
} table_t;

table_t tableCtor(size_t table_size);

void tableDtor(table_t * table);

void * tableLookup(table_t * table, const char * name);

void tableInsert(table_t * table, const char * name, void * data, size_t data_size);

#endif
