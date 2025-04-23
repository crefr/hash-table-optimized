#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

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
    uint32_t hash = MurMur32Hash(name, strlen(name), 0);
    size_t index = hash % table->table_size;

    return index;
}
