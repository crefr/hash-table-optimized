#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include "hashtable.h"
#include "word_finder.h"

#include "hash.h"

const size_t START_TABLE_SIZE = 2048;

const char * const FILE_FOR_STORE_NAME = "test_data/data_to_store.txt";
const char * const FILE_FOR_FIND_NAME  = "test_data/data_to_find.txt";

const size_t NUM_OF_CYCLES = 10;


int main(int argc, char ** argv)
{
    table_t table = tableCtor(START_TABLE_SIZE);
    printf("table size = %zu\n\n", table.table_size);

    size_t words_loaded = loadWordsIntoTable(&table, FILE_FOR_STORE_NAME);
    printf("LOADED %zu different words!\n", words_loaded);

    printf("words per bucket = %f\n\n", (float)words_loaded / (float)table.table_size);

    if (argc > 1 && strcmp("-t", argv[1]) == 0){
        printf("started finding...\n");

        printf("finding lasted %lf ms\n",
        findWordsInTable(&table, FILE_FOR_FIND_NAME, NUM_OF_CYCLES));
    }
    else if (argc > 1 && strcmp("-d", argv[1]) == 0){
        printf("deviation = %lg\n", tableTestDistribution(&table));
    }
    else {
        alignas(16) char word_to_find[128] = "";
        scanf(" %s", word_to_find);

        while(strcmp(word_to_find, "-q") != 0){
            uint32_t * search_result = (uint32_t *)tableLookup(&table, word_to_find);

            if (search_result == NULL)
                printf("There is no such word!\n");
            else
                printf("Number of '%s' = %u\n", word_to_find, *search_result);

            scanf(" %s", word_to_find);
        }
    }

    tableDtor(&table);

    return 0;
}
