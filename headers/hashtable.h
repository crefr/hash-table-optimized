#ifndef HASHTABLE_INCUDED
#define HASHTABLE_INCUDED

#include "buckets.h"
#include "strlen_strcmp_opt.h"
// #include "hash.h"

/// @brief table struct
typedef struct {
    bucket_t * buckets;
    size_t table_size;
} table_t;


const size_t NAME_ALIGNMENT = 16;

table_t tableCtor(size_t table_size);

void tableDtor(table_t * table);

void * tableLookup(table_t * table, const char * name);

void tableInsert(table_t * table, const char * name, void * data, size_t data_size);

double tableTestDistribution(table_t * table);

int tableVerifyFunc(table_t * table);

#ifndef NDEBUG
    #define tableVerify(table) tableVerifyFunc(table)
#else
    #define tableVerify(table)
#endif

#endif
