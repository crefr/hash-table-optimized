#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "hashtable.h"
#include "buckets.h"
#include "hash.h"

static size_t getBucketIndex(table_t * table, const char * name);


table_t tableCtor(size_t table_size)
{
    table_t table = {};

    table.table_size = table_size;
    table.buckets = (bucket_t *)calloc(table_size, sizeof(*table.buckets));

    for (size_t bucket_index = 0; bucket_index < table_size; bucket_index++) {
        bucketInit(&(table.buckets[bucket_index]));
    }

    return table;
}

void tableDtor(table_t * table)
{
    assert(table);

    for (size_t bucket_index = 0; bucket_index < table->table_size; bucket_index++)
        bucketDtor(&table->buckets[bucket_index]);

    free(table->buckets);
}

void * tableLookup(table_t * table, const char * name)
{
    assert(table);
    assert(name);

    size_t bucket_index = getBucketIndex(table, name);

    return bucketLookup(&(table->buckets[bucket_index]), name);
}

void tableInsert(table_t * table, const char * name, void * data, size_t data_size)
{
    assert(table);
    assert(name);
    assert(data);

    size_t bucket_index = getBucketIndex(table, name);

    bucketInsert(&(table->buckets[bucket_index]), name, data, data_size);
}

static size_t getBucketIndex(table_t * table, const char * name)
{
    // printf("addr = %p, name = %s\n", name, name);
    uint32_t hash = calcHash(name, strlen(name));
    size_t index = hash % table->table_size;

    return index;
}

double tableTestDistribution(table_t * table)
{
    assert(table);

    size_t sum   = 0;
    size_t sum_2 = 0;

    size_t table_size = table->table_size;

    for (size_t bucket_index = 0; bucket_index < table_size; bucket_index++){
        size_t size = table->buckets[bucket_index].bucket_size;

        sum   += size;
        sum_2 += size * size;
    }

    double mean_val = (double)sum   / table_size;
    double mean_sq  = (double)sum_2 / table_size;

    double dispersion = mean_sq - mean_val * mean_val;
    double sigma = sqrt(dispersion);

    return sigma;
}
