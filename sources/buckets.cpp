#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "buckets.h"

static elem_t * newElem(const char * name, void * data, size_t data_size, elem_t * next);

static void delElem(elem_t * elem);

static void bucketPush(bucket_t * bucket, const char * name, void * data, size_t data_size);


static elem_t * newElem(const char * name, void * data, size_t data_size, elem_t * next)
{
    assert(name);
    assert(data);

    elem_t * new_elem = (elem_t *)calloc(1, sizeof(*new_elem));

    strcpy(new_elem->name, name);
    new_elem->data = calloc(1, data_size);
    new_elem->data_size = data_size;
    new_elem->next = next;

    return new_elem;
}

static void delElem(elem_t * elem)
{
    assert(elem);

    free(elem->data);
    free(elem);
}

void bucketInit(bucket_t * bucket)
{
    assert(bucket);

    bucket->bucket_size = 0;
    bucket->first_elem = NULL;
}

void bucketDtor(bucket_t * bucket)
{
    assert(bucket);

    elem_t * cur_elem = bucket->first_elem;

    while(cur_elem != NULL){
        delElem(cur_elem);
        cur_elem = cur_elem->next;
    }
}

void * bucketLookup(bucket_t * bucket, const char * name)
{
    assert(bucket);
    assert(name);

    elem_t * cur_elem = bucket->first_elem;

    while (cur_elem != NULL){
        if (strcmp(name, cur_elem->name) == 0)
            return cur_elem->data;

        cur_elem = cur_elem->next;
    }

    return NULL;
}

void bucketInsert(bucket_t * bucket, const char * name, void * data, size_t data_size)
{
    assert(bucket);
    assert(name);
    assert(data);

    if (bucketLookup(bucket, name) == NULL)
        bucketPush(bucket, name, data, data_size);
}

static void bucketPush(bucket_t * bucket, const char * name, void * data, size_t data_size)
{
    assert(bucket);
    assert(name);
    assert(data);

    elem_t * new_elem = newElem(name, data, data_size, bucket->first_elem);
    bucket->first_elem = new_elem;
    bucket->bucket_size++;
}
